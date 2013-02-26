<?php
// $Id: gmap_blocks.content.inc,v 1.7 2010/05/01 11:32:12 skilip Exp $

/**
 * @file
 *
 * Include file for declaring functionality needed for an GMap Blocks block
 */

/**
 * Content callback for gmap_blocks_block().
 * Generates an array for the block implemantation, adds required css and js files
 * and prepares the javascript settings
 *
 * @param delta (number)
 *   - The block's delta which equals the block's database bid
 *
 * @see gmap_blocks_block()
 */
function gmap_blocks_content($delta) {
  $path = drupal_get_path('module', 'gmap_blocks');
  $block = gmap_blocks_block_load($delta);
  $use_api_key = (variable_get('gmap_blocks_api_key', '') !== '');

  $width = $block['settings']['width'];
  $height = $block['settings']['height'];
  $name = $block['name'];

  // Unset unneeded elements
  unset($block['settings']['width'], $block['settings']['height'], $block['name']);

  // Merge the address
  foreach ($block['locations'] as $lid => $location) {
    // If an API key is used, the latitude and longitude are stored in the database.
    // There's no need to ret these values using the address anymore.
    if ($use_api_key && !empty($location['latlng'])) {
      $block['locations'][$lid]['latlng'] = unserialize($location['latlng']);
    }
    else {
      // Create a signle address string.
      $address = array();
      foreach (array('address', 'city', 'state', 'country') as $key) {
        $address[] = $block['locations'][$lid][$key];

        // In javascript we will only use the full address string, so we can unset the rest here
        unset($block['locations'][$lid][$key]);
      }
      $block['locations'][$lid]['address'] = join(', ', $address);
    }
    $block['locations'][$lid]['marker'] = (bool) $location['marker'];
    $block['locations'][$lid]['open'] = (bool) $location['open'];

    if ($location['directions']) {
      $directions_form = drupal_get_form('gmap_blocks_directions_form', $delta, $location);
      drupal_add_css("$path/gmap_blocks.css");

      // Store the language so the google maps directions window is opened in the users language
      global $user;
      $language = user_preferred_language($user);
      $block['settings']['lang'] = $language->language;
    }

    // If a custom icon is used, create a usable url for it
    if ($location['use_icon']) {
      $block['locations'][$lid]['icon_path'] = url($block['locations'][$lid]['icon_path']);
    }
    else {
      unset($block['locations'][$lid]['icon_path']);
    }

    $info = check_markup($block['locations'][$lid]['info'], $block['locations'][$lid]['format'], FALSE);
    if (!empty($info)) {
      $block['locations'][$lid]['info'] = "<div id=\"gmap-blocks-info-window-$delta-$lid\">$info$directions_form</div>\n";
    }
  }

  // Store the id of the div element the map has to placed in by javascript
  $block['target_id'] = 'gmap-blocks-'. $delta;

  // Give other modules the opportunity to change the config on a page level
  foreach (module_implements('gmap_blocks') as $module) {
    $function = $module .'_gmap_blocks';
    $function($block);
  }

  // Check if the map exceeds Google Maps limit of locations per map.
  if (!$use_api_key && (count($block['locations']) > GMAP_API_QUERY_LIMIT)) {
    drupal_set_message(_gmap_api_query_limit_message(), 'warning');
  }

  drupal_add_js(array('gmap_blocks' => array($delta => $block)), 'setting');
  drupal_set_html_head('<script type="text/javascript" src="http://maps.google.com/maps/api/js?sensor=false"></script>');
  drupal_add_js("$path/gmap_blocks.js");

  return array(
    'subject' => $name,
    'content' => '<div id="'. $block['target_id'] .'" style="width:'. $width .'px; height:'. $height .'px"></div>',
  );
}

/**
 * Declaration of a little form used inside info windows for getting directions
 * This form will only be displayed if the user has checked the 'directions' checkbox for the location element
 */
function gmap_blocks_directions_form($form_sate, $delta, $location) {
  $form = array();

  $form['visible'] = array(
    '#prefix' => '<div class="gmap-blocks-directions-visible gmap-blocks-directions-open-link">',
    '#value' => '<a href="javascript:Drupal.GMapBlocks['. $delta .'].initDirectionsForm()">'. t('Plan route...') .'</a>',
    '#suffix' => '</div>',
  );

  $form['hidden'] = array(
    '#prefix' => '<div class="gmap-blocks-directions-hidden">',
    '#suffix' => '</div>',
  );
  $form['hidden']['source_address'] = array(
    '#type' => 'textfield',
    '#default_value' => t('Traveling from'),
    '#size' => 20,
    '#maxlength' => 128,
  );
  $form['hidden']['submit'] = array('#type' => 'submit', '#value' => t('Directions'));

  $form['direction_address'] = array('#type' => 'hidden', '#default_value' => '');
  $form['lid'] = array('#type' => 'hidden', '#default_value' => $location['lid']);
  $form['bid'] = array('#type' => 'hidden', '#default_value' => $delta);

  return $form;
}