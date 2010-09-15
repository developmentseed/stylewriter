// $Id$

/**
 * OpenLayers Zoom to Layer Behavior
 */
Drupal.behaviors.stylewriter_pointinteraction = function(context) {
  var layers, data = $(context).data('openlayers');
  if (data && data.map.behaviors['stylewriter_pointinteraction']) {
    map = data.openlayers;
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
