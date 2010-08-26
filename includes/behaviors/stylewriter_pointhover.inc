<?php
// $Id$

/**
 * @file
 * StyleWriter PointHover behavior
 */
class stylewriter_pointhover extends openlayers_behavior {
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
  
  /**
   * Render.
   */
  function render(&$map) {
    drupal_add_js(drupal_get_path('module', 'stylewriter') 
      .'/includes/behaviors/stylewriter_pointhover.js');
    return $this->options;
  }
}
