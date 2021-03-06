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
          'choropleth' => array(
            'enable' => FALSE,
            'color_min' => '',
            'color_max' => '',
            'steps' => '5',
            'ramp_type' => 'equal',
            'opacity' => 0.5,
            'special' => array(
              'enable' => FALSE
            )
          )
        ),
      );
      $options['metawriter'] = array(
        'default' => array(
          'enable' => FALSE,
          'marker-meta-writer' => 'meta1',
          'marker-meta-output' => '',
        ),
      );
      return $options;
    }

    /**
     * Form helper: get all views that can be used by
     */
    private function data_options() {
      $views = views_get_all_views();
      $options = array();
      foreach ($views as $view) {
        foreach ($view->display as $display => $data) {
          $view->set_display($display);
          if ($data->display_plugin == 'feed') {
            $options[$view->name . ':' . $display] = $view->title . 
              '(' . $view->name . ' - ' . $display . ')';
          }
        }
        $view->destroy();
      }  
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
          $field_names[$field] = ($label = $handler->label()) ? 
            $label : $handler->ui_name();
        }

        $form['fields'] = array(
          '#type' => 'fieldset',
          '#title' => 'Field usage and dataset',
          '#description' => t(''),
          '#weight' => -10,
        );
        $form['colors'] = array(
          '#type' => 'fieldset',
          '#title' => 'Colors',
          '#description' => t(''),
          '#weight' => -20,
        );
        $form['metawriter'] = array(
          '#type' => 'fieldset',
          '#title' => 'MetaWriter',
          '#description' => t(''),
          '#weight' => 10,
        );

        /*=== Stroke =======*/
        $form['colors']['stroke'] = array(
          '#type' => 'fieldset'
        );

        $form['colors']['stroke']['enable'] = array(
          '#type' => 'checkbox',
          '#title' => t('Include stroke'),
          '#id' => 'stroke-enable',
          '#default_value' => $this->options['colors']['stroke']['enable'],
          '#description' => t('Provide a stroke around points.'),
        );
        $form['colors']['stroke']['line-color'] = array(
          '#type' => 'textfield',
          '#title' => t('Stroke color'),
          '#process' => array('views_process_dependency'),
          '#dependency' => array('stroke-enable' => array('1')),
          '#default_value' => $this->options['colors']['stroke']['line-color'],
        );
        $form['colors']['stroke']['line-width'] = array(
          '#type' => 'textfield',
          '#title' => t('Stroke width'),
          '#process' => array('views_process_dependency'),
          '#dependency' => array('stroke-enable' => array('1')),
          '#default_value' => $this->options['colors']['stroke']['line-width'],
        );
        $form['colors']['stroke']['line-opacity'] = array(
          '#type' => 'textfield',
          '#title' => t('Stroke opacity'),
          '#process' => array('views_process_dependency'),
          '#dependency' => array('stroke-enable' => array('1')),
          '#default_value' => $this->options['colors']['stroke']['line-opacity'],
        );

        /*=== Color Ramp ===*/
        $form['colors']['choropleth'] = array(
          '#type' => 'fieldset'
        );
        $form['colors']['choropleth']['enable'] = array(
          '#type' => 'checkbox',
          '#title' => t('Enable Choropleth'),
          '#id' => 'choropleth-enable',
          '#default_value' => $this->options['colors']['choropleth']['enable'],
          '#description' => t('For polygon data, the StyleWriter module can 
            output polygon-fill and polygon-opacity attributes based on a value 
            in given data.')
        );
        $form['colors']['choropleth']['color_min'] = array(
          '#type' => 'textfield',
          '#title' => t('Minimum color'),
          '#process' => array('views_process_dependency'),
          '#dependency' => array('choropleth-enable' => array('1')),
          '#default_value' => $this->options['colors']['choropleth']['color_min'],
        );
        $form['colors']['choropleth']['color_max'] = array(
          '#type' => 'textfield',
          '#title' => t('Maximum color'),
          '#process' => array('views_process_dependency'),
          '#dependency' => array('choropleth-enable' => array('1')),
          '#default_value' => $this->options['colors']['choropleth']['color_max'],
        );
        $form['colors']['choropleth']['ramp_type'] = array(
          '#type' => 'select',
          '#title' => t('Ramp Type'),
          '#process' => array('views_process_dependency'),
          '#dependency' => array('choropleth-enable' => array('1')),
          '#options' => array(
            'equal' => t('Equal Intervals'),
            'quantiles' => t('Quantiles')
          ),
          '#default_value' => $this->options['colors']['choropleth']['ramp_type'],
        );
        $form['colors']['choropleth']['steps'] = array(
          '#type' => 'textfield',
          '#title' => t('Gradations'),
          '#process' => array('views_process_dependency'),
          '#dependency' => array('choropleth-enable' => array('1')),
          '#default_value' => $this->options['colors']['choropleth']['steps'],
        );
        $form['colors']['choropleth']['opacity'] = array(
          '#type' => 'textfield',
          '#title' => t('Opacity'),
          '#process' => array('views_process_dependency'),
          '#dependency' => array('choropleth-enable' => array('1')),
          '#default_value' => $this->options['colors']['choropleth']['opacity'],
        );
          $form['colors']['choropleth']['special'] = array(
            '#type' => 'fieldset'
          );
          $form['colors']['choropleth']['special']['enable'] = array(
            '#type' => 'checkbox',
            '#title' => t('Enable special'),
            '#default_value' => $this->options['colors']['choropleth']['special']['enable'],
            '#id' => 'special-enable'
          );
          $form['colors']['choropleth']['special']['value'] = array(
            '#type' => 'textfield',
            '#title' => t('Special Value'),
            '#default_value' => $this->options['colors']['choropleth']['special']['value'],
            '#process' => array('views_process_dependency'),
            '#dependency' => array('special-enable' => array('1')),
          );
          $form['colors']['choropleth']['special']['color'] = array(
            '#type' => 'textfield',
            '#title' => t('Special Color'),
            '#default_value' => $this->options['colors']['choropleth']['special']['color'],
            '#process' => array('views_process_dependency'),
            '#dependency' => array('special-enable' => array('1')),
          );
          $form['colors']['choropleth']['special']['opacity'] = array(
            '#type' => 'textfield',
            '#title' => t('Special Opacity'),
            '#default_value' => $this->options['colors']['choropleth']['special']['opacity'],
            '#process' => array('views_process_dependency'),
            '#dependency' => array('special-enable' => array('1')),
          );
          $form['colors']['choropleth']['special']['label'] = array(
            '#type' => 'textfield',
            '#title' => t('Special Label'),
            '#default_value' => $this->options['colors']['choropleth']['special']['label'],
            '#process' => array('views_process_dependency'),
            '#dependency' => array('special-enable' => array('1')),
          );

        /* -- Data Options -- */
        $form['fields']['data_strategy'] = array(
          '#type' => 'select',
          '#title' => t('Data Strategy'),
          '#options' => array(
            'grid' => t('Polygon Grid'),
            'points' => t('GeoJSON points')),
          '#default_value' => $this->options['fields']['data_strategy'],
        );
        $form['fields']['data_type'] = array(
          '#type' => 'select',
          '#title' => t('Data Type'),
          '#id' => 'data-type-select',
          '#options' => array(
            'shape' => t('Remote Shapefile'),
            'ogr' => t('GeoJSON View')),
          '#default_value' => $this->options['fields']['data_type'],
        );
          $form['fields']['data_view'] = array(
            '#type' => 'select',
            '#default_value' => $this->options['fields']['data_view'],
            '#title' => t('Data View'),
            '#process' => array('views_process_dependency'),
            '#dependency' => array('data-type-select' => array('ogr')),
            '#options' => $this->data_options(),
            '#description' => t('Data view (GeoJSON)')
          );
          $form['fields']['data_url'] = array(
            '#type' => 'textfield',
            '#default_value' => $this->options['fields']['data_url'],
            '#title' => t('Data URL'),
            '#process' => array('views_process_dependency'),
            '#dependency' => array('data-type-select' => array('shape')),
            '#description' => t('URL to the zipped shapefile or other 
              vector data source')
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
        $form['fields']['name_field'] = array(
          '#type' => 'select',
          '#title' => t('Name Field'),
          '#description' => t('Human-readable and linked name field'),
          '#options' => $field_names,
          '#default_value' => $this->options['fields']['name_field'],
        );
        $form['fields']['join_field_type'] = array(
          '#type' => 'select',
          '#title' => t('Join Field Type'),
          '#options' => array(
            'int' => t('Integer / Number'),
            'string' => t('String')
          ),
          '#description' => t('Mapnik needs numbers and strings to be formatted 
            as such when making comparisons - check the type of the join field 
            in the shapefile or GeoJSON data.'),
          '#default_value' => $this->options['fields']['join_field_type'],
        );
        $form['fields']['value'] = array(
          '#type' => 'select',
          '#title' => t('Value'),
          '#options' => $field_names,
          '#default_value' => $this->options['fields']['value'],
        );

        /*=== Scale Ramp ===*/
        $form['scale'] = array(
          '#type' => 'fieldset'
        );
        $form['scale']['enable'] = array(
          '#type' => 'checkbox',
          '#title' => t('Enable Scaled Markers'),
          '#id' => 'scale-enable',
          '#default_value' => $this->options['scale']['enable'],
          '#description' => t('Scaled markers work with point datasources 
            to provide points that get larger and smaller based on value.')
        );
          $form['scale']['min_size'] = array(
            '#type' => 'textfield',
            '#title' => t('Minimum size'),
            '#process' => array('views_process_dependency'),
            '#dependency' => array('scale-enable' => array(1)),
            '#default_value' => $this->options['scale']['min_size'],
          );
          $form['scale']['max_size'] = array(
            '#type' => 'textfield',
            '#title' => t('Maximum size'),
            '#process' => array('views_process_dependency'),
            '#dependency' => array('scale-enable' => array(1)),
            '#default_value' => $this->options['scale']['max_size'],
          );
          $form['scale']['steps'] = array(
            '#type' => 'textfield',
            '#title' => t('Gradations'),
            '#process' => array('views_process_dependency'),
            '#dependency' => array('scale-enable' => array(1)),
            '#default_value' => $this->options['scale']['steps'],
          );

        $form['postfix'] = array(
          '#type' => 'textarea',
          '#title' => t('Postfix content'),
          '#process' => array('views_process_dependency'),
          '#default_value' => $this->options['postfix'],
        );

        /*=== MetaWriter =======*/
        $form['metawriter'] = array(
          '#type' => 'fieldset'
        );
        $form['metawriter']['enable'] = array(
          '#type' => 'checkbox',
          '#title' => t('Include Metawriter'),
          '#id' => 'metawriter-enable',
          '#default_value' => $this->options['metawriter']['enable'],
          '#description' => t('MetaWriters are a capability of Mapnik 2.0 
            that allow GeoJSON to be simultaneously output with raster data')
        );
          $form['metawriter']['meta-output'] = array(
            '#type' => 'textfield',
            '#title' => t('Meta output'),
            '#process' => array('views_process_dependency'),
            '#dependency' => array('metawriter-enable' => array(1)),
            '#default_value' => $this->options['metawriter']['meta-output'],
            '#description' => t('Name the fields in the datasource that should 
              be output by the MetaWriter. In the case of Drupal-generated 
              GeoJSON, these fields might be named field_alias_rendered.')
          );
          $form['metawriter']['meta-writer'] = array(
            '#type' => 'textfield',
            '#title' => t('Meta writer'),
            '#process' => array('views_process_dependency'),
            '#dependency' => array('metawriter-enable' => array(1)),
            '#process' => array('views_process_dependency'),
            '#default_value' => $this->options['metawriter']['meta-writer'],
            '#description' => t('Name the metawriter that will be used by 
              this mapfile')
          );
      }
    }

    /**
     * @param $view_display colon-separated view-display
     *
     * return a string representing the URL of a view that returns a Mapnik style
     * with arguments inherited from the current view
     */
    private function viewurlargs($view_display) {
      global $base_url;
      list($view_name, $display_name) = explode(':', $view_display);
      if ($view = views_get_view($view_name)) {
        $args = array();
        // Retrieve args/filters from current view
        // to pass on to our vector layer.
        $current_view = views_get_current_view();
        if (isset($current_view)) {
          $args = $current_view->args;
        }
        $view->set_display($display_name);
        return $base_url . '/' . $view->get_url($args);
      }
    }

    /**
     * Execute a view and return its data
     * @param $view_display colon-separated name of a view and display
     */
    public function viewdata($view_display) {
      global $base_url;
      list($view_name, $display_name) = explode(':', $view_display);
      if ($view = views_get_view($view_name)) {
        $args = array();
        // Retrieve args/filters from current view
        // to pass on to our vector layer.
        $current_view = views_get_current_view();
        if (isset($current_view)) {
          $args = $current_view->args;
        }
        $view->set_display($display_name);
        return $view->execute();
      }
    }

    /**
     * Accessor for URL
     * @return string $url
     */
    public function data_url() {
      return $this->viewurlargs($this->options['fields']['data_view']);
    }

    /**
     * field_names
     * @return array of field names on this view
     */
    public function field_names() {
      return array_keys($this->view->field);
    }

    /**
     * Shortcut for determining whether values are special
     */
    private function is_special($value) {
      return ($this->options['colors']['choropleth']['special']['enable'] && 
        $value == $this->options['colors']['choropleth']['special']['value']);
    }

    /**
     * @param $rows the rows of a rendered view
     * @return $points all of the rows in that view which formed
     *  valid coordinates, organized into coordinates and attributes
     */
    function map_rows($rows) {
      $handlers = $this->display->handler->get_handlers('field');
      $keys = array_keys($this->view->field);
      $fields = $renders = $rendered_record = $rendered_records = array();

      foreach ($rows as $records) {
        foreach ($handlers as $hid => $handler) {
          $rendered_record[$handler->field_alias] = $handler->advanced_render($records);
        }
        $rendered_records[] = $rendered_record;
      }

      foreach ($rows as $count => $row) {
        foreach ($keys as $id) {
          $renders[$count][$id] = $row->{$this->view->field[$id]->field_alias};
        }
      }


      if ($this->options['colors']['choropleth']['enable']) {
        $points = $values = array();
        foreach ($renders as $id => $row) {
          $point = array();
          foreach ($this->view->field as $key => $field) {
            $point['join_field_name'] = $this->options['fields']['join_field_name'];
            if ($key == $this->options['fields']['join_field']) {
              $join_field_value = $row[$key];
            }
            if ($key == $this->options['fields']['value']) {
              if ($this->is_special($row[$key])) {
                $point['special'] = TRUE;
              }
              else {
                $point['value'] = $row[$key];
                $values[] = floatval($row[$key]);
              }
            }
          }
          switch ($this->options['fields']['join_field_type']) {
            case 'int':
              $point['filter'] = $point['join_field_name'] . ' = ' . $join_field_value;
              break;
            case 'string':
              $point['filter'] = $point['join_field_name'] . ' = "' . $join_field_value . '"';
              break;
          }
          $points[] = $point;
        }

        $value_min = min($values);
        $value_max = max($values);
        $value_range = $value_max - $value_min;
        if ($value_range == 0) return; // lots of divisions coming up

        $color_ramp = stylewriter_color_ramp(
          $this->options['colors']['choropleth']['color_min'], 
          $this->options['colors']['choropleth']['color_max'], 
          $this->options['colors']['choropleth']['steps']);

        foreach ($points as $i => $point) {
          if (!empty($point['special'])) {
            $points[$i]['polygon'] = array(
              'polygon-fill' => $this->options['colors']['choropleth']['special']['color'],
              'polygon-opacity' => $this->options['colors']['choropleth']['special']['opacity']);
          }
          else {
            if ($this->options['colors']['choropleth']['ramp_type'] == 'equal') {
              $rank = (floatval($point['value']) - $value_min) / $value_range; // 0 -> 1
              // Include high endpoint.
              $ramp_idx = ($rank == 1) ? 
                count($color_ramp) - 1 : 
                floor($rank * $this->options['colors']['choropleth']['steps']);
            }
            if ($this->options['colors']['choropleth']['ramp_type'] == 'quantile') {
              // TODO
            }
            $points[$i]['polygon'] = array(
              'polygon-fill' => $color_ramp[$ramp_idx],
              'polygon-opacity' => $this->options['colors']['choropleth']['opacity']);
          }
        }
      }
      elseif ($this->options['scale']['enable']) {
        $points = $values = array();

        foreach ($renders as $id => $row) {
          $point = array();
          foreach ($this->view->field as $key => $field) {
            if ($key == $this->options['fields']['value']) {
              $values[] = floatval($row[$key]);
            }
          }
        }

        $value_min = min($values);
        $value_max = max($values);
        $value_range = $value_max - $value_min;

        $value_ramp = stylewriter_color_ramp_grad(
          $value_min, 
          $value_max, 
          $this->options['scale']['steps']);

        $scale_ramp = stylewriter_color_ramp_grad(
          $this->options['scale']['min_size'], 
          $this->options['scale']['max_size'], 
          $this->options['scale']['steps']);

        for ($i = 0; $i < $this->options['scale']['steps']; $i++) {
          $points[$i]['marker'] = array(
            'marker-width' =>  intval($scale_ramp[$i]),
            'marker-height' => intval($scale_ramp[$i]));
          if ($i == 0) { 
            // lower bound
            $points[$i]['filter'] = $this->options['fields']['value'] .
              '<=' . $value_ramp[$i + 1] . '';
          }
          elseif ($i == ($this->options['scale']['steps'] - 1)) {
            // upper bound
            $points[$i]['filter'] = $this->options['fields']['value'] .
              '>=' . $value_ramp[$i];
          } else {
            $points[$i]['filter'] = $this->options['fields']['value'] .
              '>=' . $value_ramp[$i] .
              '][' . $this->options['fields']['value'] .
              '<=' . $value_ramp[$i + 1] . '';
          }
        }
      }
      return $points;
    }


    /**
     * @param $rows the rows of a rendered view
     * @return $points all of the rows in that view which formed
     *  valid coordinates, organized into coordinates and attributes
     */
    function keymap_data($rows) {
      $renders = $formatted_renders = array();
      $keys = array_keys($this->view->field);
      foreach ($rows as $count => $row) {
        foreach ($keys as $id) {
          $renders[$count][$id] = $row->{$this->view->field[$id]->field_alias};
          $formatted_renders[$count][$id] = $this->view->field[$id]->theme($row);
        }
      }
      $points = $values = array();
      foreach ($formatted_renders as $id => $row) {
        $point = array();
        foreach ($this->view->field as $key => $field) {
          switch($key) {
            case $this->options['fields']['join_field']:
              $join_field_value = $renders[$id][$key];
              break;
            case $this->options['fields']['name_field']:
              $point['name'] = $row[$key];
              break;
            case $this->options['fields']['value']:
              if ($this->is_special($renders[$id][$key])) {
                $point['description'] = $this->options['colors']['choropleth']['special']['label'];
              }
              else {
                $point['description'] = $row[$key];
              }
              break;
          }
        }
        $points[$join_field_value] = $point;
      }
      return $points;
    }

    /**
     * Return the data for a legend
     *
     * array(
     *    'points' => array()
     *    'values' => array(
     *      'min' => value
     *      'max' => value
     *    )
     * )
     */
    function legend_data($rows) {
      if ($this->options['colors']['choropleth']['enable'] || $this->options['scale']['enable']) {
        $points = $values = $formatted_values = $renders = $formatted_renders = array();
        $keys = array_keys($this->view->field);

        if ($this->options['colors']['choropleth']['enable']) {
          $steps = $this->options['colors']['choropleth']['steps'];
          $symbol_ramp = stylewriter_color_ramp(
            $this->options['colors']['choropleth']['color_min'],
            $this->options['colors']['choropleth']['color_max'],
            $this->options['colors']['choropleth']['steps']);
        }
        else {
          $steps = $this->options['scale']['steps'];
          $symbol_ramp = array_reverse(range(0,$this->options['scale']['steps'] - 1));
        }

        foreach ($rows as $count => $row) {
          foreach ($keys as $id) {
            $renders[$count][$id] = $row->{$this->view->field[$id]->field_alias};
            $formatted_renders[$count][$id] = $this->view->field[$id]->theme($row);
          }
        }
        
        foreach ($renders as $id => $row) {
          foreach ($this->view->field as $key => $field) {
            // TODO temporary hack
            // TODO getting bad invalid output here
            if ($key == $this->options['fields']['value'] &&
             !$this->is_special($row[$key])) {
              $point['value'] = $row[$key];
              $values[$id] = floatval($row[$key]);
              $formatted_values[$id] = $formatted_renders[$id][$key];
            }
          }
        }

        $value_min = min($values);
        $value_max = max($values);
        $value_range = $value_max - $value_min;
        if ($value_range == 0) return; // There are lots of divisions by zero coming up

        // Build distribution counts.
        $dist = array_fill(0, $steps, 0);
        foreach ($values as $value) {
          $rank = ($value - $value_min) / $value_range; // 0 -> 1
          // Include high endpoint.
          $ramp_idx = ($rank == 1) ? 
            count($dist) - 1 : 
            floor($rank * $steps);
          $dist[$ramp_idx] = isset($dist[$ramp_idx]) ? $dist[$ramp_idx] + 1 : 1;
        }

        // Normalize distribution counts and format as usable CSS class strings.
        $dist_min = min($dist);
        $dist_max = max($dist);
        $dist_range = $dist_max - $dist_min;

        foreach ($dist as &$d) {
          $d = str_replace('.', '', strval(round(($d - $dist_min) / $dist_range, 1)));
        }

        $value_ramp = array_map('strval', 
          stylewriter_range(
            $value_min,
            $value_max,
            ($steps + 1)));

        $legend = array(
          'points' => array(),
          'values' => array(
            'min' => $formatted_values[array_search($value_min, $values)],
            'max' => $formatted_values[array_search($value_max, $values)],
          ),
        );

        for ($i = 0; $i < $steps; $i++) {
          $legend['points'][] = array(
            'color' => $symbol_ramp[$i],
            'dist' => $dist[$i],
            'value_min' => $value_ramp[$i],
            'value_max' => $value_ramp[$i + 1],
          );
        }
        if ($this->options['scale']['enable']) {
          $legend['points'] = array_reverse($legend['points']);
        }
        return $legend;
      }
    }

    /**
     * This will return a legend for maps that are styled via 
     * stylewriter
     *
     * @param points
     * @param values
     * @param $label
     * @return $themed_html Themed HTML
     */
    function theme_legend($points, $values, $label) {
      $swatches = '';
      foreach ($points as $point) {
        if ($this->options['colors']['choropleth']['enable']) {
          $swatches .= theme('stylewriter_color_swatch', 
            $point['color'], $point['dist'], $point['value_min'], $point['value_max'], $label);
        }
        else {
          $swatches .= theme('stylewriter_point_swatch', 
            $point['color'], $point['dist'], $point['value_min'], $point['value_max'], $label);
        }
      }
      $first = current($points);
      $last = end($points);
      return theme('stylewriter_legend', 
        $swatches, 
        $values['min'], 
        $values['max'], 
        $label);
    }
}
