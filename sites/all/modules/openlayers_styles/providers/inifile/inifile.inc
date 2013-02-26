<?php

/**
 * Defines the INI file provider plugin.
 */
$plugin = array(
  'title' => t('INI file'),
  'description' => t('Loads styles from an INI file, specified by a file path.'),
  'provider' => array(
    'class' => 'openlayers_styles_ini_provider_plugin',
  ),
);
