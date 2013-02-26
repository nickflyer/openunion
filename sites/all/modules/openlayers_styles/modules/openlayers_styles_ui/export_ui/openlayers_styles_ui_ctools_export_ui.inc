<?php

/**
 * CTools export UI for OpenLayers style sources.
 */
$plugin = array(
  'schema' => 'openlayers_styles_sources',
  'access' => 'administer openlayers',

  'menu' => array(
    'menu item' => 'styles-sources',
    'menu prefix' => 'admin/build/openlayers',
    'menu title' => 'Style sources',
    'menu description' => 'Manage sources for providing OpenLayers styles.',
  ),

  'title singular' => t('style source'),
  'title plural' => t('style sources'),
  'title singular proper' => t('Style source'),
  'title plural proper' => t('Style sources'),

  'form' => array(
    'settings' => 'openlayers_styles_ui_provider_settings_form',
    'validate' => 'openlayers_styles_ui_provider_settings_form_validate',
    'submit' => 'openlayers_styles_ui_provider_settings_form_submit',
  ),
);

/**
 * Presents style provider settings form.
 */
function openlayers_styles_ui_provider_settings_form(&$form, &$form_state) {
  // Process source object.
  $source = &$form_state['item'];

  // Add basic elements.
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Administrative title'),
    '#default_value' => isset($source->title) ? $source->title : '',
    '#description' => t('The user-friendly title of this style source for administrative use.'),
    '#required' => TRUE,
  );
  $form['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Administrative description'),
    '#default_value' => isset($source->description) ? $source->description : '',
    '#description' => t('An optional detailed description of this style source for administrative use.'),
  );

  // No provider plugin has been selected.
  if (!isset($source->provider)) {
    // Prepare a list of plugins.
    $plugins = openlayers_styles_get_providers();
    $plugin_options = array();
    foreach ($plugins as $name => $plugin) {
      $plugin_options[$name] = isset($plugin['title']) ? $plugin['title'] : $name;
    }
    $form['provider'] = array(
      '#type' => 'radios',
      '#title' => t('Source type'),
      '#options' => $plugin_options,
      '#default_value' => isset($form_state['values']['provider']) ? $form_state['values']['provider'] : NULL,
      '#description' => t('The type of this source determining how styles are generated from the source. Once the type has been chosen, it cannot be changed.'),
      '#required' => TRUE,
    );
    $form['provider_new'] = array(
      '#type' => 'value',
      '#value' => TRUE,
    );
  }
  // Present the provider plugin form.
  else {
    // Display plugin title.
    $plugin = openlayers_styles_get_provider($source->provider);

    if (empty($plugin)) {
      $form['error'] = array(
        '#value' => t('Error: the module that supports this source is not enabled (type: %plugin_type).', array('%plugin_type' => $source->provider)),
      );
    }
    else {
      // Prepare provider plugin.
      $form['provider'] = array(
        '#type' => 'value',
        '#value' => $source->provider,
      );
      $form['provider_display'] = array(
        '#type' => 'item',
        '#title' => t('Source type'),
        '#value' => $plugin['title'],
      );
      $provider = openlayers_styles_create_provider($plugin);

      if (!isset($source->options)) {
        // Initialize source default options.
        $source->options = $provider->options_init();
      }

      // Display plugin form.
      $provider->options_form($form, $form_state, $source->options, $source);

      // Display testing form.
      if (function_exists('kprint_r')) {
        $form['testing'] = array(
          '#type' => 'fieldset',
          '#title' => t('Testing'),
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
        );
        $form['testing']['test_retrieve'] = array(
          '#type' => 'button',
          '#value' => t('Test retrieve'),
          '#validate' => array('openlayers_styles_ui_provider_test_retrieve'),
        );
        if (!empty($form_state['storage']['test_retrieve'])) {
          $form['testing']['#collapsed'] = FALSE;
          unset($form_state['storage']['test_retrieve']);

          // Retrieve.
          $styles = openlayers_styles_retrieve_raw_styles($source);
          $form['testing']['result'] = array(
            '#value' => kprint_r($styles, TRUE),
          );
        }
      }
    }
  }
}

/**
 * Validates style provider settings form submission.
 */
function openlayers_styles_ui_provider_settings_form_validate(&$form, &$form_state) {
  $source = &$form_state['item'];

  // Set basic values.
  foreach (array('name', 'title', 'description', 'provider') as $field) {
    if (!empty($form_state['values'][$field])) {
      $source->{$field} = $form_state['values'][$field];
    }
  }
  // Check if provider has just been selected.
  if (!empty($form_state['values']['provider_new'])) {
    // Go back and rebuild the form for the provider choice to take effect.
    $form_state['rebuild'] = TRUE;
    return;
  }
  // Set extra values.
  if ($plugin = openlayers_styles_get_provider($source->provider)) {
    $provider = openlayers_styles_create_provider($plugin);
    foreach ($provider->options_init() as $option_key => $_) {
      $source->options[$option_key] = $form_state['values'][$option_key];
    }
    // Perform extra plugin validation.
    $provider->options_form_validate($form, $form_state, $source->options, $source);
  }
}

/**
 * Tests source retrieval.
 */
function openlayers_styles_ui_provider_test_retrieve(&$form, &$form_state) {
  $form_state['storage']['test_retrieve'] = TRUE;
  $form_state['rebuild'] = TRUE;
}

/**
 * Finalizes style provider settings form submission.
 */
function openlayers_styles_ui_provider_settings_form_submit(&$form, &$form_state) {
  openlayers_styles_clear_styles_cache($form_state['item']->name);
}
