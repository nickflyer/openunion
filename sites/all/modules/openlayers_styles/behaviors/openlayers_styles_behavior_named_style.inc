<?php

/**
 * Provides named styles settings form.
 */
class openlayers_styles_behavior_named_style extends openlayers_behavior {
  /**
   * Provide initial values for options.
   */
  function options_init() {
    return array(
      'named_style_layers' => array(),
    );
  }

  /**
   * Provide name attribute settings form for all vector layers.
   */
  function options_form($defaults) {
    $form = array();

    $form['help'] = array(
      '#type' => 'item',
      '#title' => t('Style name attribute'),
      '#value' => t('For each layer, enter a comma separated list of attribute fields (available for features in the layer) to use as style names (unique IDs) for variable styles. The first non-empty attribute found will be used.'),
    );
    $form['named_style_layers'] = array(
      '#type' => 'fieldset',
      '#title' => t('Layers'),
      '#tree' => TRUE,
    );

    foreach ($this->map['layers'] as $id => $name) {
      $layer = openlayers_layer_load($id);
      if (!empty($layer->data['vector'])) {
        $form['named_style_layers'][$id] = array(
          '#type' => 'textfield',
          '#title' => $name,
          '#default_value' => isset($defaults['named_style_layers'][$id]) ? $defaults['named_style_layers'][$id] : '',
        );
      }
    }

    return $form;
  }

  /**
   * Alter styling of each features in specified layers.
   */
  function render(&$map) {
    $styles = openlayers_styles();
    $named_style_layers = (array) $this->options['named_style_layers'];
    foreach ($named_style_layers as $id => $style_names) {
      $names = array_map('trim', explode(',', $style_names));
      foreach ($map['layers'][$id]['features'] as &$feature) {
        // Scan non-empty attributes for name.
        foreach ($names as $name) {
          if (!empty($feature['attributes'][$name])) {
            $style_name = $feature['attributes'][$name];
            if (isset($styles[$style_name]) && !empty($styles[$style_name]->data)) {
              $feature['style'] = $styles[$style_name]->data;
            }
            break;
          }
        }
      }
      unset($feature);
    }

    return $this->options;
  }
}
