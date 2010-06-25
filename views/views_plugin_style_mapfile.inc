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
  /*
    function init(&$view, &$display, $options = NULL) {
      parent::init($view, $display, $options = NULL);
    }
   */

    /**
     * Set default options
     */
    function option_definition() {
      $options = parent::option_definition();
      $options['fields'] = array(
        'default' => array(
          'join_field_name' => '',
          'join_field' => '',
          'value' => '',
        ),
      );
      $options['colors'] = array(
        'default' => array(
          'color_min' => '',
          'color_max' => '',
          'steps' => '',
          'opacity' => 0.5,
        ),
      );
      return $options;
    }

    /**
     * Provide a form for setting options.
     *
     * @param array $form
     * @param array $form_state
     */
    function options_form(&$form, &$form_state) { 
      parent::options_form($form, $form_state);

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

        $form['colors']['opacity'] = array(
          '#type' => 'textfield',
          '#title' => t('Opacity'),
          '#default_value' => $this->options['colors']['opacity'],
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
          $renders[$count][$id] = $row->{$this->view->field[$id]->field_alias};
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
          // TODO temporary hack
          // TODO getting bad invalid output here
          if ($key == $this->options['fields']['value']) {
            $point['value'] = $row[$key];
            $values[] = floatval($row[$key]);
          }
        }
        $points[] = $point;
      }

      $value_min = min($values);
      $value_max = max($values);
      $value_range = $value_max - $value_min;

      $color_ramp = stylewriter_color_ramp(
        $this->options['colors']['color_min'], 
        $this->options['colors']['color_max'], 
        $this->options['colors']['steps']);

      foreach ($points as $i => $point) {
        $rank = (floatval($point['value']) - $value_min) / $value_range; // 0 -> 1
        // TODO: floor or ceil?
        // range is 0-(steps - 1)
        $ramp_idx = floor($rank * ($this->options['colors']['steps'] - 1));
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
      $formatted_renders = array();
      $keys = array_keys($this->view->field);

      foreach ($rows as $count => $row) {
        foreach ($keys as $id) {
          $renders[$count][$id] = $row->{$this->view->field[$id]->field_alias};
        }
      }

      foreach ($rows as $count => $row) {
        foreach ($keys as $id) {
          $formatted_renders[$count][$id] = $this->view->field[$id]->theme($row);
        }
      }  

      $points = array();
      $values = array();

      
      foreach ($renders as $id => $row) {
        foreach ($this->view->field as $key => $field) {
          // TODO temporary hack
          // TODO getting bad invalid output here
          if ($key == $this->options['fields']['value']) {
            $point['value'] = $row[$key];
            $values[] = floatval($row[$key]);
          }
        }
      }

      $value_min = min($values);
      $value_max = max($values);

      $value_ramp = stylewriter_range(
        $value_min, 
        $value_max, 
        $this->options['colors']['steps']);

      $dist = array_fill(0, $this->options['colors']['steps'], 0);

      for ($i = 0; $i < ($this->options['colors']['steps'] - 1); $i++) {
        foreach ($values as $value) {
          if ($value >= $value_ramp[$i] && $value < $value_ramp[$i + 1]) {
            $dist[$i] = (isset($dist[$i])) ? $dist[$i] + 1 : 1;
          }
          elseif ($value == $value_ramp[$i + 1]) {
            $dist[$i + 1] = (isset($dist[$i + 1])) ? $dist[$i + 1] + 1 : 1;
          }
        }
      }

      // normalize distributions
      $dist_min = min($dist);
      $dist_max = max($dist);
      foreach ($dist as &$d) {
        $d = str_replace('.', '', strval(round(($d - $dist_min) / (($dist_max - $dist_min) / 10), 1)));
      }

      $color_ramp = stylewriter_color_ramp(
        $this->options['colors']['color_min'], 
        $this->options['colors']['color_max'], 
        $this->options['colors']['steps']);

      $value_ramp = array_map('strval', $value_ramp);
     
      $legend = array();

      for ($i = 0; $i < $this->options['colors']['steps']; $i++) {
        $legend[] = array(
          'color' => $color_ramp[$i],
          'dist' => $dist[$i],
          'value' => $value_ramp[$i]);
      }

      return $legend;
    }

    function theme_legend($points) {
      $swatches = '';
      $first = current($points);
      $last = end($points);
      foreach ($points as $point) {
        switch ($point) {
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
          $point['value'],
          $point['color'],
          $point['dist'],
          $place
          );
      }
      return theme('stylewriter_legend', $swatches);
    }
}
