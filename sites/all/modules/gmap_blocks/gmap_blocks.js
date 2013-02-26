// $Id: gmap_blocks.js,v 1.9 2010/05/01 11:59:51 skilip Exp $

// Create a container for all GMapBlocks instances
Drupal.GMapBlocks = {};

// After the document has fully loaded, we'll loop through the Drupal.settings.gmaps_block array.
// For each element in the array, a new instance of GMapBlicks will be created.
$(function() {
  if (Drupal.settings && Drupal.settings.gmap_blocks) {
    for (var delta in Drupal.settings.gmap_blocks) {
      Drupal.GMapBlocks[delta] = new GMapBlock(Drupal.settings.gmap_blocks[delta]);
      Drupal.GMapBlocks[delta].init();
    };
  };
});

/**
 * Creates a new Google Map instance
 *
 * @param params (object)
 *   - Instance block settings
 */
function GMapBlock(params) {
  var self = this;

  self.params;
  self.locations;
  self.settings;
  self.target_obj;
  self.bid;
  self.settings;
  self.map;
  self.geocoder;

  /**
   * Initializes a GMapBlock instance
   */
  self.init = function() {
    self.params = params;
    self.locations = params.locations;
    self.settings = self.params.settings;
    self.target_obj = $('#' + params.target_id).get(0);
    self.bid = self.params.bid;

    self.parseMapSettings();
    self.initGeocoder();
    self.initMap();
    self.parseLocations();
  };

  /**
   * Initializes a gmap Map instance
   */
  self.initMap = function() {
    self.map = new google.maps.Map(self.target_obj, self.settings);
  };

  /**
   * Initializes a gmap Geocoder instance
   */
  self.initGeocoder = function() {
    self.geocoder = new google.maps.Geocoder();
  };

  /**
   * Since the values in the settings object, passed by php, can only contain strings,
   * we need to eval the elements which should contain jasvascript variables
   */
  self.parseMapSettings = function() {
    self.settings.mapTypeId = eval('google.maps.MapTypeId.' + self.settings.mapTypeId);

    if (self.settings.mapTypeControl) {
      self.settings.mapTypeControlOptions.style = eval('google.maps.MapTypeControlStyle.' + self.settings.mapTypeControlOptions.style);
    };

    if (self.settings.navigationControl) {
      self.settings.navigationControlOptions.style = eval('google.maps.NavigationControlStyle.' + self.settings.navigationControlOptions.style);
    };
  };

  /**
   * Loops through all locations for this instance and creates a new location on the map
   */
  self.parseLocations = function() {
    for (var lid in self.locations) {
      self.setLocation(lid);
    };
  };

  /**
   * Sets a new location on the map
   * Depending on the location properties, a marker and an info window will be created
   *
   * @param lid (number)
   *   - The location id
   */
  self.setLocation = function(lid) {

    // If an api key exsists, request the location over HTTP, since this is way faster.
    if (typeof(self.locations[lid].latlng) == 'object') {
      self.locations[lid].latlng = new google.maps.LatLng(self.locations[lid].latlng.lat, self.locations[lid].latlng.lng);
      self.parseLocationData(lid);
    }
    else {
      self.codeAddress(self.locations[lid].address, function(result) {
        self.locations[lid].latlng = result[0].geometry.location;
        self.parseLocationData(lid);
      });
    };
  };

  /**
   *
   * @param lid (number)
   *   - The location id
   */
  self.parseLocationData = function(lid) {
    if (lid == self.settings.center_location) {
      self.map.setCenter(self.locations[lid].latlng);
    };

    if (self.locations[lid].marker) {
      self.locations[lid].marker = self.createMarker(self.locations[lid].latlng, self.locations[lid].address, self.locations[lid].icon_path);
    };

    if (self.locations[lid].info !== '') {
      self.locations[lid].infoWindow = self.createInfoWindow(self.locations[lid].info);

      if (lid == self.settings.center_location && self.locations[lid].open) {
        self.locations[lid].infoWindow.open(self.map, self.locations[lid].marker);
      };

      google.maps.event.addListener(self.locations[lid].marker, 'click', function() {
        self.locations[lid].infoWindow.open(self.map, self.locations[lid].marker);
      });
    };
  };

  /**
   * Get's the latlng object by a given search address
   *
   * @param address (string)
   *   - The address to search for. E.g.: 'Death valley 69'
   *
   * @param callback (function)
   *   - A callback function for when the latlng object is found
   */
  self.codeAddress = function(address, callback) {
    if (self.geocoder) {
      self.geocoder.geocode({address: address}, function(results, status) {
        if (status == google.maps.GeocoderStatus.OK) {
          if (status != google.maps.GeocoderStatus.ZERO_RESULTS) {
            callback(results);
          }
          else {
            alert(Drupal.t("No results found"));
          }
        }
        else {
          alert(Drupal.t("Geocode could not find '!address' for the following reason: !status", {'!status': status, '!address': address}));
        };
      });
    };
  };

  /**
   * Function to create an info window
   */
  self.createInfoWindow = function(contentString) {
    return new google.maps.InfoWindow({content: contentString});
  };

  /**
   * A function to create a gmap marker
   *
   * @param latlng (object)
   *   - The latlng object generated by gmap Geocoder
   *
   * @param title (string)
   *   - The title displayed when hovering the marker
   *
   * @param icon (string)
   *   - Path to a custom icon image
   *
   * @return (object)
   *   - A new Marker inctance
   */
  self.createMarker = function(latlng, title, icon) {
    var marker = {
        position: latlng, 
        map: self.map, 
        title: title
    };

    if (icon) {
      marker.icon = icon;
    };
    return new google.maps.Marker(marker);
  };

  /**
   * Unhides the directions form in info windows and initializes the form's functionality
   */
  self.initDirectionsForm = function() {
    var form  = $('input[@name=bid][@value=' + self.bid + ']').parents('form');
    var lid  = $('input[@name=lid]', form).val();

    $('.gmap-blocks-directions-visible, .gmap-blocks-directions-hidden', form).toggleClass('gmap-blocks-directions-hidden').toggleClass('gmap-blocks-directions-visible');

    $('.form-text', form).focus(function() {
      $(this).val('').css({'color': 'black'});
    });
    
    $('.form-text', form).focus(function() {
      $(this).val('').css({'color': 'black'});
    });
    $('input[@name=direction_address]', form).val(self.locations[lid].address);

    form.submit(function() {
      var direction_address = $('input[@name=direction_address]', $(this)).val();
      var source_address = $('input[@name=source_address]', $(this)).val();
      var lid  = $('input[@name=lid]', $(this)).val();

      $('.gmap-blocks-directions-visible', $(this)).show();
      $('.gmap-blocks-directions-hidden', $(this)).css({'visibility': 'hidden'});

      self.locations[lid].infoWindow.close();
      self.openDirectionsWindow(source_address, direction_address, 800, 600, self.settings.lang);

      return false;
    });
  };

  /**
   * Opens a direction on maps.google.com in a popup window.
   *
   * @param saddr (string)
   *   - The direction source address
   *
   * @param daddr (string)
   *   - The direction address
   *
   * @param width (number)
   *   - The width of the popup
   *
   * @param height (number)
   *   - The height of the popup
   */
  self.openDirectionsWindow = function(saddr, daddr, width, height, lang) {
    var left = parseInt((screen.availWidth / 2) - (width / 2));
    var top = parseInt((screen.availHeight / 2) - (height / 2));

    saddr.replace(/ /, '+');
    daddr.replace(/ /, '+');
  
    window.open('http://maps.google.com/maps?saddr=' + saddr + '&daddr=' + daddr + '&hl=' + lang, 'gmb_directions', 'width=' + width + ',height=' + height + ',top=' + top + ',left=' + left);
  };

  return self;
};
