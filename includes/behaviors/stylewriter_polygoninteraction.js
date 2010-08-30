// $Id$

/**
 * Drupal behavior driver
 */
Drupal.behaviors.stylewriter_polygoninteraction = function(context) {
  var layers, data = $(context).data('openlayers');
  if (data && data.map.behaviors['stylewriter_polygoninteraction']) {
    map = data.openlayers;
    layer = map.getLayersBy('drupalID', 
      data.map.behaviors.stylewriter_polygoninteraction.layer)[0];

    h = new OpenLayers.Control.PolygonInteraction({
      layer: layer,
      join_field: data.map.behaviors.stylewriter_polygoninteraction.join_field,
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
