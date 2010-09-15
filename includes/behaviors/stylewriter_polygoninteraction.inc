<?php
// $Id$

/**
 * @file
 * StyleWriter Polygon Interaction behavior
 */
class stylewriter_polygoninteraction extends openlayers_behavior {
  /**
   * Override of options_init().
   */
  function options_init() {
    $options = parent::options_init();
    return $options;
  }

  /**
   * Override of options_form().
   */
  function options_form() {
    $form = parent::options_form();
    $form['layer'] = array(
      '#type' => 'select',
      '#multiple' => TRUE,
      '#options' => $this->map['layers'],
      '#default_value' => $this->options['layer']
    );
    return $form;
  }

  function js_dependency() {
    return array(
      'OpenLayers.Handler.Hover',
      'OpenLayers.Handler.Click');
  }
  
  /**
   * Render.
   */
  function render(&$map) {
    drupal_add_js(drupal_get_path('module', 'stylewriter') 
      .'/includes/behaviors/jquery.jsonp-2.1.2.min.js');
    drupal_add_js(drupal_get_path('module', 'stylewriter') 
      .'/includes/behaviors/stylewriter_tooltip.js');
    drupal_add_js(drupal_get_path('module', 'stylewriter') 
      .'/includes/behaviors/stylewriter_openlayers_polygoninteraction.js');
    drupal_add_js(drupal_get_path('module', 'stylewriter') 
      .'/includes/behaviors/stylewriter_polygoninteraction.js');
    return $this->options;
  }
}
