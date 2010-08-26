<?php
// $Id$

/**
 * @file
 * StyleWriter GridHover behavior
 */
class stylewriter_gridhover extends openlayers_behavior {
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
    $layer = openlayers_layer_load($this->options['layer']);
    $this->options['join_field'] = $layer->get_joinfield();

    drupal_add_js(drupal_get_path('module', 'stylewriter') 
      .'/includes/behaviors/stylewriter_gridhover.js');
    return $this->options;
  }
}

