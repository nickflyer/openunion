// $Id: gmap_blocks.admin.js,v 1.2 2010/01/25 14:58:54 sutharsan Exp $

$(function() {
  $('input[@name=disableDefaultUI]').click(function() {
    if ($(this).attr('checked')) {
      if ($('#control_set').hasClass('collapsed')) {
        Drupal.toggleFieldset($('#control_set'));
      };
    }
    else {
      if (!$('#control_set').hasClass('collapsed')) {
        Drupal.toggleFieldset($('#control_set'));
      };
    };
  });
});