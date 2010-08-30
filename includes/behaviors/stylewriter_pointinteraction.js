// $Id$

/**
 * OpenLayers Zoom to Layer Behavior
 */
Drupal.behaviors.stylewriter_pointhover = function(context) {
  var layers, data = $(context).data('openlayers');
  if (data && data.map.behaviors['stylewriter_pointhover']) {
    map = data.openlayers;
    layer = map.getLayersBy('drupalID', 
      data.map.behaviors['stylewriter_pointhover'].layer)[0];
    h = new OpenLayers.Control.PointInteraction({
      layer: layer,
      callbacks: {
        'over': Drupal.StyleWriterTooltips.select,
        'out': Drupal.StyleWriterTooltips.unselect,
        'click': Drupal.StyleWriterTooltips.click
      }
    });
    map.addControl(h);
    h.activate();
  }
};
