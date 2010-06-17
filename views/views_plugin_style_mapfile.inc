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

        $form['colors'] = array(
          '#type' => 'fieldset',
          '#title' => 'Colors',
          '#description' => t(''),
          '#weight' => -20,
        );
        
        $form['colors']['color_min'] = array(
          '#type' => 'textfield',
          '#title' => t('Minimum color'),
          '#default_value' => $this->options['colors']['color_min'],
        );
   
        $form['colors']['color_max'] = array(
          '#type' => 'textfield',
          '#title' => t('Maximum color'),
          '#default_value' => $this->options['colors']['color_max'],
        );
         
        $form['colors']['steps'] = array(
          '#type' => 'textfield',
          '#title' => t('Gradations'),
          '#default_value' => $this->options['colors']['steps'],
        );

        $form['fields']['join_field_name'] = array(
          '#type' => 'textfield',
          '#title' => t('Join Field Name'),
          '#default_value' => $this->options['fields']['join_field_name'],
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
      $values = array();

      foreach ($renders as $id => $row) {
        $point = array();

        foreach ($this->view->field as $key => $field) {
          $point['join_field_name'] = $this->options['fields']['join_field_name'];
          if ($key == $this->options['fields']['join_field']) {
            $point['join_field_value'] = $row[$key];
          }
          // echo "[{$key}]";
          // TODO temporary hack
          // TODO getting bad invalid output here
          if ($key == $this->options['fields']['value']) {
            $point['value'] = $row[$key];
            $values[] = intval($row[$key]);
          }
        }

        $points[] = $point;
      }

      $value_min = min($values);
      $value_max = max($values);

      $color_ramp = stylewriter_color_ramp(
        $this->options['colors']['color_min'], 
        $this->options['colors']['color_max'], 
        $this->options['colors']['steps']);

      foreach ($points as $i => $point) {
        $rank = ($point['value'] - $value_min) / $value_max; // 0 -> 1
        // TODO: floor or ceil?
        $ramp_idx = floor($rank * $this->options['colors']['steps']);
        $points[$i]['color'] = $color_ramp[$ramp_idx];
      }

      return $points;
    }


    /**
     * Return the data for a legend
     *
     * array(
     *    'data_type' => array(
     *    'color' => value
     *    'color' => value
     *    'color' => value
     *    )
     *    )
     */
    function legend_data($rows) {
      $renders = array();
      $keys = array_keys($this->view->field);
      foreach ($rows as $count => $row) {
        foreach ($keys as $id) {
          $renders[$count][$id] = $this->view->field[$id]->theme($row);
        }
      }  
      $points = array();
      $values = array();
      foreach ($renders as $id => $row) {
        $point = array();
        foreach ($this->view->field as $key => $field) {
          if ($key == $this->options['fields']['value']) {
            $point['value'] = $row[$key];
            $values[] = floatval($row[$key]);
          }
        }
        $points[] = $point;
      }

      $value_min = min($values);
      $value_max = max($values);

      $color_ramp = stylewriter_color_ramp(
        $this->options['colors']['color_min'], 
        $this->options['colors']['color_max'], 
        $this->options['colors']['steps']);

      $value_ramp = stylewriter_range(
        $value_min, 
        $value_max, 
        $this->options['colors']['steps']);

      $value_ramp = array_map('strval', $value_ramp);
     
      return array_combine($value_ramp, $color_ramp);
    }

    function theme_legend($points) {
      $swatches = '';
      $first = current($points);
      $last = end($points);
      foreach ($points as $value => $color) {
        switch ($color) {
          case $first:
            $place = 'first';
            break;
          case $last:
            $place = 'last';
            break;
          default:
            $place = 'mid';
            break;
        }
        $swatches .= theme('stylewriter_swatch', 
          $value,
          $color,
          $place
          );
      }
      return theme('stylewriter_legend', $swatches);
    }
}
