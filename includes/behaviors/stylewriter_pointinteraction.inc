<?php
// $Id$

/**
 * @file
 * StyleWriter Point Interaction behavior
 */
class stylewriter_pointinteraction extends openlayers_behavior {
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
      '#options' => $this->map['layers'],
      '#default_value' => $this->options['layer']
    );
    return $form;
  }

  function js_dependency() {
    return array(
      'OpenLayers.Handler.Hover',
      'OpenLayers.Handler.Click'
    );
  }
  
  /**
   * Render.
   */
  function render(&$map) {
    drupal_add_js(drupal_get_path('module', 'stylewriter') 
      .'/includes/behaviors/stylewriter_tooltip.js');
    drupal_add_js(drupal_get_path('module', 'stylewriter') 
      .'/includes/behaviors/stylewriter_openlayers_pointinteraction.js');
    drupal_add_js(drupal_get_path('module', 'stylewriter') 
      .'/includes/behaviors/stylewriter_pointinteraction.js');
    return $this->options;
  }
}
