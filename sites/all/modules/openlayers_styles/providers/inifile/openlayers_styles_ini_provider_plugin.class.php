<?php

/**
 * Provides sources from an INI file.
 */
class openlayers_styles_ini_provider_plugin extends openlayers_styles_provider_plugin {
  /**
   * Provides default options.
   */
  function options_init() {
    return array(
      'path' => NULL,
    );
  }

  /**
   * Configures INI file path.
   */
  function options_form(&$form, &$form_state, $options = array(), $source = NULL) {
    $form['path'] = array(
      '#type' => 'textfield',
      '#title' => t('INI file path'),
      '#default_value' => $options['path'],
      '#description' => t('The path or URL to the INI file. The path can be a local path relative to the Drupal site base. Begin the URL with "http://" or "https://" to specify a web URL. Begin "public://" to specify a path under the default file system path for this site.'),
      '#required' => TRUE,
    );
    return $form;
  }

  /**
   * Validates file path.
   */
  function options_form_validate(&$form, &$form_state, $options = array(), $source = NULL) {
    $path = $this->expand_path($options['path']);
    if (!file_exists($path) && !valid_url($path, TRUE)) {
      form_set_error('path', t('The specified path is invalid.'));
    }
  }

  /**
   * Loads and parses the INI file.
   */
  function retrieve($options, $source) {
    if (empty($options['path'])) {
      return;
    }

    // Load file.
    $ini = $this->load_ini_file($options['path']);
    if (isset($ini)) {
      // Parse INI configuration into styles.
      $entries = $this->parse_ini($ini);
      $entries = $this->prepare_entries($entries, $options);
      $styles = $this->process_entries($entries, $options);

      return $styles;
    }
  }

  /**
   * Load an INI file into a string.
   */
  function load_ini_file($path) {
    try {
      return $this->load_file($path);
    }
    catch (Exception $ex) {
      // Return nothing if an exception is caught.
    }
  }

  /**
   * Load a file by a parameterized URL.
   */
  function load_file($path) {
    $contents = NULL;
    $path = $this->expand_path($path);
    if (file_exists($path)) {
      // Load file with default behavior.
      $contents = $this->load_local_file($path);
    }
    elseif (valid_url($path, TRUE)) {
      // Load HTTP URL.
      $contents = $this->load_http_file($path);
    }
    else {
      // Abort for invalid format.
      throw new InvalidArgumentException(t('Invalid path.'));
    }
    return $contents;
  }

  /**
   * Expand a path or URL into a local file path or an HTTP URL.
   */
  function expand_path($path) {
    if (!file_exists($path)) {
      $parsed_url = @parse_url($path);
      if (!empty($parsed_url) && !empty($parsed_url['scheme'])) {
        // Expand "public://" paths.
        if ($parsed_url['scheme'] == 'public') {
          return file_directory_path() . '/' . substr($path, 9);
        }
      }
    }
    // Return default path.
    return $path;
  }

  /**
   * Loads a file from a local path.
   */
  function load_local_file($path) {
    return file_get_contents($path);
  }

  /**
   * Loads a file from a remote URL.
   */
  function load_http_file($path) {
    // Retrieve file from web URL.
    $result = drupal_http_request($path);
    if (in_array($result->code, array(200, 302))) {
      return $result->data;
    }
    else {
      // No result.
      throw new UnexpectedValueException(t('Unexpected HTTP error.'));
    }
  }

  /**
   * Parses an INI configuration string into an array structure.
   */
  function parse_ini($ini) {
    $entries = array();
    $section = NULL;
    $subsection = NULL;

    $lines = preg_split('/(\r|\n)+/', $ini);
    foreach ($lines as $line) {
      $line = trim($line);
      // Ignore empty and commented lines.
      if ($line == '' || @$line[0] == ';') {
        continue;
      }

      // Match section.
      if ($line[0] == '[') {
        if (substr($line, -1) != ']' || '' === $section_text = trim(substr($line, 1, -1))) {
          // Skip if section is not properly named.
          continue;
        }
        else {
          if (isset($subsection)) {
            // Commit previous subsection.
            $entries[$section] = $subsection;
          }
          // Reset for new section.
          $section = $section_text;
          $subsection = NULL;
        }
      }
      // Match entry.
      elseif (preg_match('/^([^=]*)=\s*(.*)$/', $line, $match)) {
        $key = trim($match[1]);
        $value = trim($match[2]);

        // Parse PHP-style deep array keys.
        preg_match('/^([^\[]+)((?:\[[^\]]*\])*)$/', $key, $key_match);
        $keys = array($key_match[1]);
        if ($key_match[2] !== '' && preg_match_all('/\[([^\]]*)\]/', $key_match[2], $subkey_matches)) {
          foreach ($subkey_matches[1] as $subkey) {
            $keys[] = $subkey;
          }
        }

        // Cast numeric value to specific type.
        if (is_numeric($value)) {
          // Cast as integer.
          if (intval($value) == floatval($value)) {
            $value = (int) $value;
          }
          // Cast as float.
          else {
            $value = (float) $value;
          }
        }

        // Extract value enclosed in double quotes.
        if ($value[0] == '"' && substr($value, -1) == '"') {
          $value = stripslashes(substr($value, 1, -1));
        }

        // Initialize subsection.
        if (!isset($subsection)) {
          $subsection = array();
        }
        // Set value depending on how far down the keys go.
        $handle = &$subsection;
        foreach ($keys as $subkey) {
          if ($subkey === '') {
            $handle = &$handle[];
          }
          else {
            $handle = &$handle[$subkey];
          }
        }
        $handle = $value;
      }
    }

    // Commit final subsection.
    if (isset($subsection)) {
      $entries[$section] = $subsection;
    }

    return $entries;
  }

  /**
   * Preprocess an INI structure.
   */
  function prepare_entries($entries, $options) {
    if (isset($entries[NULL]) || isset($entries['defaults'])) {
      // Prepare default values.
      $defaults = array();
      foreach (array(NULL, 'defaults') as $defaults_key) {
        if (!empty($entries[$defaults_key])) {
          foreach ($entries[$defaults_key] as $key => $value) {
            $defaults[$key] = $value;
            // Remove from entries.
            unset($entries[$defaults_key]);
          }
        }
      }
      // Process remaining entries.
      foreach ($entries as $name => $entry) {
        $entries[$name] += $defaults;
      }
    }

    return $entries;
  }

  /**
   * Process an INI structure into styles.
   */
  function process_entries($entries, $options) {
    $styles = array();
    $entries = $this->prepare_entries($entries);
    $hard_defaults = array(
      'api_version' => 1,
    );
    $basic_keys = array('api_version', 'name', 'title', 'description');
    $required_keys = array('title');

    foreach ($entries as $name => $entry) {
      // Populate style object properties.
      $style = new stdClass();
      $style->name = $name;
      $data = array();
      $entry += $hard_defaults;
      foreach ($entry as $key => $value) {
        if (in_array($key, $basic_keys)) {
          // Set basic object value.
          $style->$key = $value;
        }
        else {
          // Set object data value.
          $data[$key] = $value;
        }
      }
      $style->data = $data;

      // Validate style object before pushing.
      foreach ($required_keys as $key) {
        if (!isset($style->$key)) {
          // Skip invalid style object.
          continue 2;
        }
      }

      $style = $this->process_style($style, $options);
      $styles[$name] = $style;
    }

    return $styles;
  }

  /**
   * Process a style entry being retrieved.
   */
  function process_style($style, $options) {
    // Normalize icon URL.
    if (!empty($style->data['externalGraphic'])) {
      $graphic_url = &$style->data['externalGraphic'];
      $path = $this->expand_path($options['path']);
      if (file_exists($path)) {
        $graphic_url = openlayers_styles_graphic_url($graphic_url, array(
          'prefix' => dirname($path) . '/',
        ));
      }
      elseif (valid_url($path, TRUE)) {
        $graphic_url = openlayers_styles_graphic_url($graphic_url, array(
          'base_url' => dirname($path),
        ));
      }
    }
    return $style;
  }
}
