<?php
// $Id: gmap_blocks.admin.inc,v 1.6 2010/05/01 11:59:51 skilip Exp $

/**
 * @file
 *
 * Include file for defining all administrive forms and functionalty
 */

/**
 * A list of all GMap Blocks
 */
function gmap_blocks_overview_page($type = 'block') {
  $type_block = ($type == 'block');
  $data = ($type_block ? gmap_blocks_get_blocks() : gmap_blocks_get_locations());
  $path = "admin/build/gmap_blocks/". ($type_block ? 'blocks' : 'locations');

  foreach ($data as $row) {
    $id = $row[($type_block ? 'bid' : 'lid')];
    $name = ($type_block ? $row['name'] : theme('gmap_blocks_location_title', $row));

    $rows[] = array(
      check_plain($name),
      l(t('edit'), "$path/$id/edit"),
      l(t('delete'), "$path/$id/delete"),
    );
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No data yet')), array('data' => l(t('Add !type', array('!type' => t($type))), "$path/add"), 'colspan' => '2'));
  }

  return theme('table', array(t('Name'), array('data' => t('operation'), 'colspan' => '2')), $rows);
}

/**
 * The block create / edit form
 */
function gmap_blocks_block_form($form_state, $edit = array()) {
  $form = array();
  $form['#cache'] = TRUE;

  // Set some default settings
  $edit += array(
    'settings' => array(
      'mapTypeId' => 'ROADMAP',
      'width' => 400,
      'height' => 300,
      'zoom' => 13,
      'scrollwheel' => TRUE,
      'disableDefaultUI' => FALSE,
      'navigationControl' => FALSE,
      'mapTypeControl' => FALSE,
      'scaleControl' => FALSE,
    ),
    'locations' => array(),
  );

  if (isset($form_state['storage']) && isset($form_state['storage']['block'])) {
    $edit = $form_state['storage']['block'];
  }

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => $edit['name'],
    '#required' => TRUE,
  );

  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['options']['mapTypeId'] = array(
    '#type' => 'radios',
    '#title' => t('Map type'),
    '#options' => array(
      'ROADMAP' => _form_control_description('mapType', t('ROADMAP')),
      'SATELLITE' => _form_control_description('mapType', t('SATELLITE')),
      'HYBRID' => _form_control_description('mapType', t('HYBRID')),
      'TERRAIN' => _form_control_description('mapType', t('TERRAIN')),
    ),
    '#default_value' => $edit['settings']['mapTypeId'],
    '#description' => t('In addition to the familiar "painted" road map tiles, the Google Maps API also supports other maps types.'),
  );
  for ($i = 1; $i <= 20; $i++) {
    $zoom_options[$i] = $i;
  }
  $form['options']['zoom'] = array(
    '#type' => 'select',
    '#title' => t('Zoom level'),
    '#options' => $zoom_options,
    '#description' => t('The zoom level defines the resolution of the GMap view.'),
    '#default_value' => $edit['settings']['zoom'],
  );
  $form['options']['scrollwheel'] = array(
    '#type' => 'checkbox',
    '#title' => t('Scrollwheel zooming'),
    '#description' => t('If unchecked, scrollwheel zooming is disabled on the map.'),
    '#default_value' => $edit['settings']['scrollwheel'],
  );
  dpm($edit);

  $form['dimensions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Dimensions'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['dimensions']['width'] = array(
    '#type' => 'textfield',
    '#title' => t('Width'),
    '#size' => 4,
    '#description' => t('The width of the block'),
    '#default_value' => $edit['settings']['width'],
  );
  $form['dimensions']['height'] = array(
    '#type' => 'textfield',
    '#title' => t('Height'),
    '#size' => 4,
    '#description' => t('The height of the block'),
    '#default_value' => $edit['settings']['height'],
  );

  $form['controls'] = array(
    '#type' => 'fieldset',
    '#title' => t('Controls'),
    '#collapsible' => TRUE,
    '#collapsed' => !($edit['settings']['disableDefaultUI']),
  );
  $form['controls']['disableDefaultUI'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use this custom control set'),
    '#default_value' => $edit['settings']['disableDefaultUI'],
    '#description' => t('Check this option to create a custom control set. Otherwise the default GMap User Interface will be used.'),
  );

  $form['controls']['control_set'] = array(
    '#type' => 'fieldset',
    '#title' => t('Custom Control Set'),
    '#attributes' => array('id' => 'control_set'),
    '#collapsible' => TRUE,
    '#collapsed' => !($edit['settings']['disableDefaultUI']),
  );

  $form['controls']['control_set']['navigationControlStyle'] = array(
    '#type' => 'radios',
    '#title' => t('Navigation control style'),
    '#options' => array(
      t('Disabled'),
      'SMALL' => _form_control_description('navigationControl', t('SMALL')),
      'ZOOM_PAN' => _form_control_description('navigationControl', t('ZOOM_PAN')),
      'ANDROID' => _form_control_description('navigationControl', t('ANDROID')),
      'DEFAULT' => _form_control_description('navigationControl', t('DEFAULT')),
    ),
    '#default_value' => ($edit['settings']['navigationControl'] ? $edit['settings']['navigationControlOptions']['style'] : 0),
    '#description' => t('The Navigation control displays a large pan/zoom control as used on Google Maps. This control appears in the top left corner of the map. You can adjust the size of this control within the Map options object upon setup.'),
  );

  $form['controls']['control_set']['mapTypeControlStyle'] = array(
    '#type' => 'radios',
    '#title' => t('Map type control'),
    '#options' => array(
      t('Disabled'),
      'HORIZONTAL_BAR' => _form_control_description('mapTypeControl', t('HORIZONTAL_BAR')),
      'DROPDOWN_MENU' => _form_control_description('mapTypeControl', t('DROPDOWN_MENU')),
      'DEFAULT' => _form_control_description('mapTypeControl', t('DEFAULT')),
    ),
    '#default_value' => ($edit['settings']['mapTypeControl'] ? $edit['settings']['mapTypeControlOptions']['style'] : 0),
    '#description' => t('The MapType control lets the user toggle between map types (such as ROADMAP and SATELLITE). You can adjust the behavior of this control through the Map options object upon setup.'),
  );
  $form['controls']['control_set']['scaleControl'] = array(
    '#type' => 'radios',
    '#title' => t('Scale control'),
    '#options' => array(
      t('Disabled'),
      t('Enabled'),
    ),
    '#default_value' => (integer) $edit['settings']['scaleControl'],
    '#description' => t('The Scale control displays a map scale element.'),
  );

  // Create the select list options for adding a location.
  $location_options = array();
  foreach (gmap_blocks_get_locations() as $location) {
    $location_options[$location['lid']] = theme('gmap_blocks_location_title', $location);
  }

  // Loop through each attached locations
  foreach (array_keys($edit['locations']) as $lid) {
    // Create radio options for setting the center location
    $radio_options[$lid] = '';

    // If no center_location is set yet, set the first one as default
    if (!isset($edit['settings']['center_location']) || !isset($edit['locations'][$edit['settings']['center_location']])) {
      $edit['settings']['center_location'] = $lid;
    }

    // Create a remove button for all locations
    $form["remove-$lid"] = array('#type' => 'submit', '#value' => t('Remove'), '#name' => "remove-$lid");

    // We don't wan't this location to be added twice
    // so we'll unset it from the select list options
    unset($location_options[$lid]);
  }

  // For setting the center location
  $form['center_location'] = array(
    '#type' => 'radios',
    '#options' => $radio_options,
    '#default_value' => $edit['settings']['center_location'],
  );

  $form['location_select'] = array(
    '#prefix' => '<div class="clear-block location-select">',
    '#suffix' => '</div>',
    '#weight' => 4,
  );

  if (!empty($location_options)) {
    $form['location_select']['location'] = array(
      '#type' => 'select',
      '#options' => $location_options,
    );
    $form['location_select']['add'] = array('#type' => 'submit', '#value' => t('Add location'));
  }
  else {
    $form['location_select']['create_new_location'] = array(
      '#value' => t('Click !link to create a new location.', array('!link' => l(t('here'), 'admin/build/gmap_blocks/locations/add', array('query' => drupal_get_destination())))),
    );
  }

  $form['bid'] = array('#type' => 'value', '#value' => $edit['bid']);
  $form['locations'] = array('#type' => 'value', '#value' => $edit['locations']);

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'), '#weight' => 8);
  $form['delete'] = array('#type' => 'submit', '#value' => t('Delete'), '#weight' => 10);

  return $form;
}

/**
 * Submit callback
 */
function gmap_blocks_block_form_submit($form, &$form_state) {

  if (empty($form_state['values']['op'])) {
    // Check if the clicked button was a 'Remove location' button
    $clicked_button = $form_state['clicked_button']['#name'];
    preg_match("/^([a-z]+)-(\d+)$/", $clicked_button, $matches);

    if ($matches[1] == 'remove' && is_numeric($matches[2])) {
      $lid = $matches[2];

      $block = gmap_blocks_form_submit_fetch_values($form_state);
      unset($block['locations'][$lid]);

      $form_state['storage']['block'] = $block;
    }
  }
  else {
    // Regular submit
    switch ($form_state['values']['op']) {

      case t('Add location'):
        $lid = $form_state['values']['location'];

        $block = gmap_blocks_form_submit_fetch_values($form_state);
        $block['locations'][$lid] = gmap_blocks_location_load($lid);

        $form_state['storage']['block'] = $block;
        break;

      case t('Delete'):
        $form_state['redirect'] = 'admin/build/gmap_blocks/blocks/'. $form_state['values']['bid'] .'/delete';
        break;

      case t('Save'):

        // Check if the block contains more than ten locations. If so, the block needs to load it's locations using a HTTP request,
        // in order to avoid OVER_QUERY_LIMIT errors. For the HTTP request, an API key is compulsory, so we check here if the API key is set.
        if (count($form_state['values']['locations']) > GMAP_API_QUERY_LIMIT && variable_get('gmap_blocks_api_key', '') == '') {
          drupal_set_message(_gmap_api_query_limit_message(), 'warning');
        }

        // Create an array to pass to gmap_blocks_block_save().
        $block = gmap_blocks_form_submit_fetch_values($form_state, TRUE);

        gmap_blocks_block_save($block);
        unset($form_state['storage']);
        $form_state['redirect'] = 'admin/build/gmap_blocks';
        break;

      default:
        break;
    }
  }
}

/**
 * Form defenition for deleting a block or location.
 */
function gmap_blocks_delete_form(&$form_state, $type = NULL, $data = NULL) {

  if (!empty($data) && ($type == 'block' || $type == 'location')) {
    $replacements = array('!type' => t($type), '%name' => (($type == 'block') ? $data['name'] : theme('gmap_blocks_location_title', $data)));

    $form['id'] = array('#type' => 'value', '#value' => ($type == 'block' ? $data['bid'] : $data['lid']));
    $form['type'] = array('#type' => 'value', '#value' => $type);
    $form['name'] = array('#type' => 'value', '#value' => $name);
    $form['replacements'] = array('#type' => 'value', '#value' => $replacements);

    return confirm_form($form, t('Are you sure you want to delete !type %name', $replacements), 'admin/build/gmap_blocks', t('By confirming to delete the !type %name, all data is lost. This action can not be undone!', $replacements));
  }
  drupal_set_message(t('No valid input. Please return to the !link', array('!link' => l(t('overview'), 'admin/build/gmap_blocks'))));
}

/**
 * Submit callback for deleting a block or location.
 */
function gmap_blocks_delete_form_submit($form_id, &$form_state) {
  if (isset($form_state['values']['type'])) {
    // Delete the entry from the database
    if ($form_state['values']['type'] == 'block') {
      db_query("DELETE FROM {gmap_blocks_blocks} WHERE bid = %d", $form_state['values']['id']);
      db_query("DELETE FROM {gmap_blocks_block_locations} WHERE bid = %d", $form_state['values']['id']);

      // Update the 'blocks' DB table with the blocks currently exported by modules
      _block_rehash();
      $form_state['redirect'] = 'admin/build/gmap_blocks/blocks';
    }
    else {
      db_query("DELETE FROM {gmap_blocks_locations} WHERE lid = %d", $form_state['values']['id']);
      db_query("DELETE FROM {gmap_blocks_block_locations} WHERE lid = %d", $form_state['values']['id']);
      $form_state['redirect'] = 'admin/build/gmap_blocks/locations';
    }

    drupal_set_message(t('The !type %name has been deleted!', $form_state['values']['replacements']));
  }
  else {
    form_set_error(t('Couldn\'t delete !type %name!', $form_state['values']['replacements']));
  }
}

/**
 * The location create / edit form
 */
function gmap_blocks_location_form($form_state, $edit = array()) {
  $form = array();

  $edit += array(
    'marker' => 1,
  );

  $form['#attributes']['enctype'] = 'multipart/form-data';

  $form['address'] = array(
    '#type' => 'textfield',
    '#title' => t('Address'),
    '#required' => TRUE,
    '#default_value' => $edit['address'],
  );
  $form['city'] = array(
    '#type' => 'textfield',
    '#title' => t('City'),
    '#required' => TRUE,
    '#default_value' => $edit['city'],
  );
  $form['state'] = array(
    '#type' => 'textfield',
    '#title' => t('State'),
    '#default_value' => $edit['state'],
  );
  $form['country'] = array(
    '#type' => 'select',
    '#title' => t('Country'),
    '#default_value' => $edit['country'],
    '#options' => counties_options(),
  );
  $form['marker'] = array(
    '#type' => 'checkbox',
    '#title' => t('Marker'),
    '#description' => t('Markers identify locations on the map'),
    '#default_value' => $edit['marker'],
  );
  $form['icon'] = array(
    '#type' => 'fieldset',
    '#title' => t('Marker icon'),
    '#collapsible' => TRUE,
    '#collapsed' => !($edit['use_icon']),
  );
  $form['icon']["use_icon"] = array(
    '#type' => 'checkbox',
    '#title' => t('Use a custom icon'),
    '#default_value' => $edit['use_icon'],
    '#tree' => FALSE,
    '#description' => t('Check this if you want this icon to be used instead of Google\'s default marker.')
  );
  $form['icon']['icon_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path to the icon'),
    '#default_value' => $edit['icon_path'],
    '#field_prefix' => url(NULL, array('absolute' => TRUE)),
    '#description' => t('The path to the image you would like to use as an icon for this location.'),
  );
  $form['icon']['icon_upload'] = array(
    '#type' => 'file',
    '#title' => t('Upload a new icon image'),
    '#maxlength' => 40,
    '#description' => t("If you don't have direct file access to the server, use this field to upload your icon.")
  );
  $form['info_window'] = array(
    '#type' => 'fieldset',
    '#title' => t('Info window'),
    '#collapsible' => TRUE,
    '#collapsed' => (!empty($edit['info'])),
    '#description' => t('Here you can add some text or HTML to add to an overlaying info balloon'),
  );
  $form['info_window']['info'] = array(
    '#type' => 'textarea',
    '#title' => t('Content'),
    '#default_value' => $edit['info'],
  );
  $form['info_window']['format'] = filter_form($edit['format']);

  $form['info_window']['directions'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use Directions'),
    '#description' => t('If checked, a directions form will be added to the info window.'),
    '#default_value' => $edit['directions'],
  );

  $form['info_window']['open'] = array(
    '#type' => 'checkbox',
    '#title' => t('Open on load'),
    '#description' => t('If checked, the info window will be opened on page load.'),
    '#default_value' => $edit['open'],
  );

  if (isset($edit['lid'])) {
    $form['lid'] = array('#type' => 'value', '#value' => $edit['lid']);
  }

  if (isset($_GET['destination']) && !empty($_GET['destination'])) {
    $form['destination'] = array('#type' => 'value', '#value' => $_GET['destination']);
  }

  $form['latlng'] = array('#type' => 'value', '#default_value' => '');

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  return $form;
}

/**
 * Validation handler for the location form
 */
function gmap_blocks_location_form_validate($form, &$form_state) {
  if (variable_get('gmap_blocks_api_key', '') !== '') {
    $values = $form_state['values'];
    $result = gmap_blocks_validate_location($values['address'], $values['city'], $values['state'], $values['country']);

    if (is_int($result)) {
      form_set_error('', _geocoder_error_code_messages($result));
    }
    else {
      $form_state['values']['latlng'] = serialize($result);
    }

    // Validate the icon path.
    if (!empty($form_state['values']['icon_path']) && !file_exists($form_state['values']['icon_path'])) {
      form_set_error('icon_path', t('The icon image can not be found. Check your entry or upload an icon first.'));
    }
  }
}

/**
 * Submit handler for the location form
 * Saves the edited location into the database
 */
function gmap_blocks_location_form_submit($form, &$form_state) {

  // Check for a new uploaded icon
  if ($file = file_save_upload('icon_upload', array('file_validate_is_image' => array()))) {

    // The image was saved using file_save_upload() and was added to the
    // files table as a temporary file. We'll make a copy and let the garbage
    // collector delete the original upload.
    file_copy($file, $file->filename, FILE_EXISTS_REPLACE);
    $form_state['values']['icon_path'] = $file->filepath;
    $form_state['values']['use_icon'] = TRUE;
  }

  gmap_blocks_location_save($form_state['values']);
  $form_state['redirect'] = (isset($form_state['values']['destination']) ? $form_state['values']['destination'] : 'admin/build/gmap_blocks/locations');
}

/**
 * Theme implementation for gmap_blocks_block_form().
 */
function theme_gmap_blocks_block_form($form) {

  // Create a table for all attached locations
  foreach ($form['locations']['#value'] as $lid => $location) {
    $rows[] = array(
      theme('gmap_blocks_location_title', $location),
      drupal_render($form['center_location'][$lid]),
      l(t('edit'), "admin/build/gmap_blocks/locations/$lid/edit", array('query' => drupal_get_destination())),
      drupal_render($form["remove-$lid"]),
    );
  }
  if (empty($rows)) {
    $rows[0] = array(array('data' => t('No locations added yet!'), 'colspan' => 3));
  }
  $form['locations_list'] = array(
    '#value' => theme('table', array(t('locations'), t('center'), array('data' => t('operations'), 'colspan' => 2)), $rows),
    '#weight' => 2,
  );

  $path = drupal_get_path('module', 'gmap_blocks');
  drupal_add_css("$path/gmap_blocks.css");
  drupal_add_js("$path/gmap_blocks.admin.js");

  return drupal_render($form);
}

/**
 * Creates an $block array instance from $form_state
 *
 * @param $form_state (array)
 *   - The form_state array containing the values we need to parse
 *
 * @param $type_convert (boolean)
 *   - Whether or not type conversion needs to be done. This is used before actually saving the data
 */
function gmap_blocks_form_submit_fetch_values($form_state) {
  return array(
    'bid' => $form_state['values']['bid'],
    'name' => $form_state['values']['name'],
    'settings' => array(
      'mapTypeId' => $form_state['values']['mapTypeId'],
      'width' => $form_state['values']['width'],
      'height' => $form_state['values']['height'],
      'zoom' => (integer) $form_state['values']['zoom'],
      'scrollwheel' => (integer) $form_state['values']['scrollwheel'],
      'disableDefaultUI' => (bool) $form_state['values']['disableDefaultUI'],
      'navigationControl' => (bool) $form_state['values']['navigationControlStyle'],
      'navigationControlOptions' => array('style' => $form_state['values']['navigationControlStyle']),
      'mapTypeControl' => (bool) $form_state['values']['mapTypeControlStyle'],
      'mapTypeControlOptions' => array('style' => $form_state['values']['mapTypeControlStyle']),
      'scaleControl' => (bool) $form_state['values']['scaleControl'],
      'center_location' => $form_state['values']['center_location'],
    ),
    'locations' => $form_state['values']['locations'],
  );
}

/**
 * The overall settings form
 */
function gmap_blocks_settings_page() {
  $gmap_api_link = 'http://code.google.com/intl/nl-NL/apis/maps/signup.html';

  $form['gmap_blocks_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('API Key'),
    '#default_value' => variable_get('gmap_blocks_api_key', ''),
    '#description' => t('If a GMap Block is created, and the block contains more than !limit locations, using an API Key is compulsory. You can read more about GMap API keys, or create one at !link', array('!limit' => GMAP_API_QUERY_LIMIT, '!link' => l($gmap_api_link, $gmap_api_link, array('external' => TRUE)))),
    '#required' => TRUE,
  );
  return system_settings_form($form);
}

/**
 * Helper function for determine wheter or not a location exists.
 *
 * The function requires a number of address elements to look for.
 * With the given address, a HTTP request will be done at maps.google.com.
 * If a location is found, an json formatted string will be returned,
 * which is used to find the location's longitude and latitude.
 *
 * @return
 *   - On succes: (array) The array containing the longitude and latitude.
 *   - On on error: (int) The error code which can be used with _geocoder_error_code_messages() to display a message.
 */
function gmap_blocks_validate_location() {
  $args = func_get_args(); // Let the user define which address elements to use for arguments.

  // Create the URL.
  $url  = 'http://maps.google.com/maps/geo?q='. drupal_urlencode(join('+', $args));
  $url .= '&sensor=false&output=json&key='. variable_get('gmap_blocks_api_key', '');

  // Try to get the json string for this location over a HTTP request.
  if ($json = @file_get_contents($url)) {
    $data = json_decode($json, TRUE);

    // If the data is loaded successfully, return the coordinates array.
    // Otherwise return the error code.
    if ($data['Status']['code'] == 200) {
      return array('lng' => $data['Placemark'][0]['Point']['coordinates'][0], 'lat' => $data['Placemark'][0]['Point']['coordinates'][1]);
    }
    else {
      return (int) $data['Status']['code'];
    }
  }
  else {
    // The file could not be read
    // Return an error code.
    return 100;
  }
}

/**
 * Helper function for the countries select list options
 *
 * @see gmap_blocks_block_form()
 */
function counties_options() {
  return array(
    'Afghanistan' => 'Afghanistan',
    'Albania' => 'Albania',
    'Algeria' => 'Algeria',
    'American Samoa' => 'American Samoa',
    'Andorra' => 'Andorra',
    'Angola' => 'Angola',
    'Anguilla' => 'Anguilla',
    'Antarctica' => 'Antarctica',
    'Antigua and Barbuda' => 'Antigua and Barbuda',
    'Argentina' => 'Argentina',
    'Armenia' => 'Armenia',
    'Aruba' => 'Aruba',
    'Australia' => 'Australia',
    'Austria' => 'Austria',
    'Azerbaijan' => 'Azerbaijan',
    'Bahamas' => 'Bahamas',
    'Bahrain' => 'Bahrain',
    'Bangladesh' => 'Bangladesh',
    'Barbados' => 'Barbados',
    'Belarus' => 'Belarus',
    'Belgium' => 'Belgium',
    'Belize' => 'Belize',
    'Benin' => 'Benin',
    'Bermuda' => 'Bermuda',
    'Bhutan' => 'Bhutan',
    'Bolivia' => 'Bolivia',
    'Bosnia and Herzegovina' => 'Bosnia and Herzegovina',
    'Botswana' => 'Botswana',
    'Bouvet Island' => 'Bouvet Island',
    'Brazil' => 'Brazil',
    'British Indian Ocean Territory' => 'British Indian Ocean Territory',
    'Brunei Darussalam' => 'Brunei Darussalam',
    'Bulgaria' => 'Bulgaria',
    'Burkina Faso' => 'Burkina Faso',
    'Burundi' => 'Burundi',
    'Cambodia' => 'Cambodia',
    'Cameroon' => 'Cameroon',
    'Canada' => 'Canada',
    'Cape Verde' => 'Cape Verde',
    'Cayman Islands' => 'Cayman Islands',
    'Central African Republic' => 'Central African Republic',
    'Chad' => 'Chad',
    'Chile' => 'Chile',
    'China' => 'China',
    'Christmas Island' => 'Christmas Island',
    'Cocos Islands' => 'Cocos Islands',
    'Colombia' => 'Colombia',
    'Comoros' => 'Comoros',
    'Congo' => 'Congo',
    'Congo, Democratic Republic of the' => 'Congo, Democratic Republic of the',
    'Cook Islands' => 'Cook Islands',
    'Costa Rica' => 'Costa Rica',
    'Cote d\'Ivoire' => 'Cote d\'Ivoire',
    'Croatia' => 'Croatia',
    'Cuba' => 'Cuba',
    'Cyprus' => 'Cyprus',
    'Czech Republic' => 'Czech Republic',
    'Denmark' => 'Denmark',
    'Djibouti' => 'Djibouti',
    'Dominica' => 'Dominica',
    'Dominican Republic' => 'Dominican Republic',
    'Ecuador' => 'Ecuador',
    'Egypt' => 'Egypt',
    'El Salvador' => 'El Salvador',
    'Equatorial Guinea' => 'Equatorial Guinea',
    'Eritrea' => 'Eritrea',
    'Estonia' => 'Estonia',
    'Ethiopia' => 'Ethiopia',
    'Falkland Islands' => 'Falkland Islands',
    'Faroe Islands' => 'Faroe Islands',
    'Fiji' => 'Fiji',
    'Finland' => 'Finland',
    'France' => 'France',
    'French Guiana' => 'French Guiana',
    'French Polynesia' => 'French Polynesia',
    'Gabon' => 'Gabon',
    'Gambia' => 'Gambia',
    'Georgia' => 'Georgia',
    'Germany' => 'Germany',
    'Ghana' => 'Ghana',
    'Gibraltar' => 'Gibraltar',
    'Greece' => 'Greece',
    'Greenland' => 'Greenland',
    'Grenada' => 'Grenada',
    'Guadeloupe' => 'Guadeloupe',
    'Guam' => 'Guam',
    'Guatemala' => 'Guatemala',
    'Guinea' => 'Guinea',
    'Guinea-Bissau' => 'Guinea-Bissau',
    'Guyana' => 'Guyana',
    'Haiti' => 'Haiti',
    'Heard Island and McDonald Islands' => 'Heard Island and McDonald Islands',
    'Honduras' => 'Honduras',
    'Hong Kong' => 'Hong Kong',
    'Hungary' => 'Hungary',
    'Iceland' => 'Iceland',
    'India' => 'India',
    'Indonesia' => 'Indonesia',
    'Iran' => 'Iran',
    'Iraq' => 'Iraq',
    'Ireland' => 'Ireland',
    'Israel' => 'Israel',
    'Italy' => 'Italy',
    'Jamaica' => 'Jamaica',
    'Japan' => 'Japan',
    'Jordan' => 'Jordan',
    'Kazakhstan' => 'Kazakhstan',
    'Kenya' => 'Kenya',
    'Kiribati' => 'Kiribati',
    'Kuwait' => 'Kuwait',
    'Kyrgyzstan' => 'Kyrgyzstan',
    'Laos' => 'Laos',
    'Latvia' => 'Latvia',
    'Lebanon' => 'Lebanon',
    'Lesotho' => 'Lesotho',
    'Liberia' => 'Liberia',
    'Libya' => 'Libya',
    'Liechtenstein' => 'Liechtenstein',
    'Lithuania' => 'Lithuania',
    'Luxembourg' => 'Luxembourg',
    'Macao' => 'Macao',
    'Macedonia' => 'Macedonia',
    'Madagascar' => 'Madagascar',
    'Malawi' => 'Malawi',
    'Malaysia' => 'Malaysia',
    'Maldives' => 'Maldives',
    'Mali' => 'Mali',
    'Malta' => 'Malta',
    'Marshall Islands' => 'Marshall Islands',
    'Martinique' => 'Martinique',
    'Mauritania' => 'Mauritania',
    'Mauritius' => 'Mauritius',
    'Mayotte' => 'Mayotte',
    'Mexico' => 'Mexico',
    'Micronesia' => 'Micronesia',
    'Moldova' => 'Moldova',
    'Monaco' => 'Monaco',
    'Mongolia' => 'Mongolia',
    'Montserrat' => 'Montserrat',
    'Morocco' => 'Morocco',
    'Mozambique' => 'Mozambique',
    'Myanmar' => 'Myanmar',
    'Namibia' => 'Namibia',
    'Nauru' => 'Nauru',
    'Nepal' => 'Nepal',
    'Netherlands' => 'Netherlands',
    'Netherlands Antilles' => 'Netherlands Antilles',
    'New Caledonia' => 'New Caledonia',
    'New Zealand' => 'New Zealand',
    'Nicaragua' => 'Nicaragua',
    'Niger' => 'Niger',
    'Nigeria' => 'Nigeria',
    'Norfolk Island' => 'Norfolk Island',
    'North Korea' => 'North Korea',
    'Norway' => 'Norway',
    'Oman' => 'Oman',
    'Pakistan' => 'Pakistan',
    'Palau' => 'Palau',
    'Palestinian Territory' => 'Palestinian Territory',
    'Panama' => 'Panama',
    'Papua New Guinea' => 'Papua New Guinea',
    'Paraguay' => 'Paraguay',
    'Peru' => 'Peru',
    'Philippines' => 'Philippines',
    'Pitcairn' => 'Pitcairn',
    'Poland' => 'Poland',
    'Portugal' => 'Portugal',
    'Puerto Rico' => 'Puerto Rico',
    'Qatar' => 'Qatar',
    'Romania' => 'Romania',
    'Russian Federation' => 'Russian Federation',
    'Rwanda' => 'Rwanda',
    'Saint Helena' => 'Saint Helena',
    'Saint Kitts and Nevis' => 'Saint Kitts and Nevis',
    'Saint Lucia' => 'Saint Lucia',
    'Saint Pierre and Miquelon' => 'Saint Pierre and Miquelon',
    'Saint Vincent and the Grenadines' => 'Saint Vincent and the Grenadines',
    'Samoa' => 'Samoa',
    'San Marino' => 'San Marino',
    'Sao Tome and Principe' => 'Sao Tome and Principe',
    'Saudi Arabia' => 'Saudi Arabia',
    'Senegal' => 'Senegal',
    'Serbia and Montenegro' => 'Serbia and Montenegro',
    'Seychelles' => 'Seychelles',
    'Sierra Leone' => 'Sierra Leone',
    'Singapore' => 'Singapore',
    'Slovakia' => 'Slovakia',
    'Slovenia' => 'Slovenia',
    'Solomon Islands' => 'Solomon Islands',
    'Somalia' => 'Somalia',
    'South Africa' => 'South Africa',
    'South Georgia' => 'South Georgia',
    'South Korea' => 'South Korea',
    'Spain' => 'Spain',
    'Sri Lanka' => 'Sri Lanka',
    'Sudan' => 'Sudan',
    'Suriname' => 'Suriname',
    'Svalbard and Jan Mayen' => 'Svalbard and Jan Mayen',
    'Swaziland' => 'Swaziland',
    'Sweden' => 'Sweden',
    'Switzerland' => 'Switzerland',
    'Syrian Arab Republic' => 'Syrian Arab Republic',
    'Taiwan' => 'Taiwan',
    'Tajikistan' => 'Tajikistan',
    'Tanzania' => 'Tanzania',
    'Thailand' => 'Thailand',
    'Timor-Leste' => 'Timor-Leste',
    'Togo' => 'Togo',
    'Tokelau' => 'Tokelau',
    'Tonga' => 'Tonga',
    'Trinidad and Tobago' => 'Trinidad and Tobago',
    'Tunisia' => 'Tunisia',
    'Turkey' => 'Turkey',
    'Turkmenistan' => 'Turkmenistan',
    'Tuvalu' => 'Tuvalu',
    'Uganda' => 'Uganda',
    'Ukraine' => 'Ukraine',
    'United Arab Emirates' => 'United Arab Emirates',
    'United Kingdom' => 'United Kingdom',
    'United States' => 'United States',
    'United States Minor Outlying Islands' => 'United States Minor Outlying Islands',
    'Uruguay' => 'Uruguay',
    'Uzbekistan' => 'Uzbekistan',
    'Vanuatu' => 'Vanuatu',
    'Vatican City' => 'Vatican City',
    'Venezuela' => 'Venezuela',
    'Vietnam' => 'Vietnam',
    'Virgin Islands, British' => 'Virgin Islands, British',
    'Virgin Islands, U.S.' => 'Virgin Islands, U.S.',
    'Wallis and Futuna' => 'Wallis and Futuna',
    'Western Sahara' => 'Western Sahara',
    'Yemen' => 'Yemen',
    'Zambia' => 'Zambia',
    'Zimbabwe' => 'Zimbabwe',
  );
}

/**
 * Helper function for descriptions in the radio options inside the gmap blocks block form
 *
 * @param type (string)
 *   - The key of the radios form element
 *
 * @param title (string)
 *   - The title of the radio option
 *
 * @return (string)
 *   - Markup containing the title and description for the radio option.
 *
 * @see gmap_blocks_block_form()
 */
function _form_control_description($type, $title) {
  $descriptions = array(
    'navigationControl' => array(
      t('SMALL') => t('Displays a mini-zoom control, consisting of only + and - buttons. This style is appropriate for mobile devices.'),
      t('ZOOM_PAN') => t('Displays the standard zoom slider control with a panning control, as on Google Maps.'),
      t('ANDROID') => t('Displays the small zoom control as used on the native Maps application on Android devices.'),
      t('DEFAULT') => t('Picks an appropriate navigation control based on the map\'s size and the device on which the map is running.'),
    ),
    'mapTypeControl' => array(
      t('HORIZONTAL_BAR') => t('Displays the array of controls as buttons in a horizontal bar as is shown on Google Maps.'),
      t('DROPDOWN_MENU') => t('Displays a single button control allowing you to select the map type via a dropdown menu.'),
      t('DEFAULT') => t('Displays the "default" behavior, which depends on screen size and may change in future versions of the API'),
    ),
    'mapType' => array(
      t('ROADMAP') => t('Displays the normal, default 2D tiles of Google Maps.'),
      t('SATELLITE') => t('Displays photographic tiles.'),
      t('HYBRID') => t('Displays a mix of photographic tiles and a tile layer for prominent features (roads, city names).'),
      t('TERRAIN') => t('Displays physical relief tiles for displaying elevation and water features (mountains, rivers, etc.).'),
    ),
  );
  return "$title <span class=\"description\">". $descriptions[$type][$title] ."</span>";
}

/**
 * Helper function for displaying a proper message to the user when a location object
 * could not be loaded after a HTTP lookup due to an error.
 */
function _geocoder_error_code_messages($code) {
  $status_messages = array(
    10 => t('The file required for determining the locations longitude and latitude could not be opened!'),
    500 => t('A geocoding or directions request could not be successfully processed, yet the exact reason for the failure is unknown.'),
    601 => t('An empty address was specified in the HTTP q parameter.'),
    602 => t('No corresponding geographic location could be found for the specified address, possibly because the address is relatively new, or because it may be incorrect.'),
    603 => t('The geocode for the given address or the route for the given directions query cannot be returned due to legal or contractual reasons.'),
    610 => t('The given key is either invalid or does not match the domain for which it was given.'),
    620 => t('The given key has gone over the requests limit in the 24 hour period or has submitted too many requests in too short a period of time. If you\'re sending multiple requests in parallel or in a tight loop, use a timer or pause in your code to make sure you don\'t send the requests too quickly.'),
  );
  return $status_messages[$code];
}

/**
 * Theme funtion for creating a location title.
 */
function theme_gmap_blocks_location_title($location) {
  return $location['address'] .", ". $location['city'];
}