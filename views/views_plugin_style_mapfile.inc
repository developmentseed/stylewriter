<?php
// $Id$

/**
 * @file
 * Extending the view_plugin_style class to provide a mapfile view style.
 */
class views_plugin_style_mapfile extends views_plugin_style {

  /**
   * Initialize plugin.
   *
   * Set feed image for shared rendering later.
   */
  function init(&$view, &$display, $options = NULL) {
    parent::init($view, $display, $options = NULL);
  }

  /**
   * Provide a form for setting options.
   *
   * @param array $form
   * @param array $form_state
   */
  function options_form(&$form, &$form_state) { 
    parent::options_form($form, $form_state);                  
    $options = parent::option_definition();

    $handlers = $this->display->handler->get_handlers('field');
    if (empty($handlers)) {
      $form['error_markup'] = array(
        '#value' => t('You need at least one field before you can 
        configure your field settings'),
        '#prefix' => '<div class="error form-item description">',
        '#suffix' => '</div>',
      );
    }
    else {
      // Data Source options
      $field_names[$field] = array('' => '--');
      foreach ($handlers as $field => $handler) {
        if ($label = $handler->label()) {
          $field_names[$field] = $label;
        }
        else {
          $field_names[$field] = $handler->ui_name();
        }
      }

      $form['fields'] = array(
        '#type' => 'fieldset',
        '#title' => 'Field usage',
        '#description' => t(''),
        '#weight' => -10,
      );

      $form['fields']['join_field'] = array(
        '#type' => 'select',
        '#title' => t('Join Field'),
        '#options' => $field_names,
        '#default_value' => $this->options['fields']['join_field'],
      );

      $form['fields']['value'] = array(
        '#type' => 'select',
        '#title' => t('Value'),
        '#options' => $field_names,
        '#default_value' => $this->options['fields']['value'],
      );
    }
  }

  function field_names() {
    return array_keys($this->view->field);
  }

  /**
   * @param $rows the rows of a rendered view
   * @return $points all of the rows in that view which formed
   *  valid coordinates, organized into coordinates and attributes
   */
  function map_rows($rows) {
    // Fields must be rendered in order as of Views 2.3, 
    // so we will pre-render everything.
    
    $renders = array();
    $keys = array_keys($this->view->field);

    foreach ($rows as $count => $row) {
      foreach ($keys as $id) {
        $renders[$count][$id] = $this->view->field[$id]->theme($row);
      }
    }  

    $points = array();

    foreach ($renders as $id => $row) {
      $point = array();

      foreach ($this->view->field as $key => $field) {
        if ($key == $this->options['fields']['join_field']) {
          $point['join_field'] = $row[$key];
        }
        if ($key == $this->options['fields']['value']) {
          $point['value'] = $row[$key];
        }
      }
      $points[] = $point;
    }
    print_r($points);
    return $points;
  }
}
