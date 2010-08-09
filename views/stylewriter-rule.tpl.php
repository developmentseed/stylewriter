<?php
// $Id$
/**
 * Style a Mapnik Rule
 *
 * @param $join_field_name
 * @param $join_field_value
 *
 * @param $color
 * @param $opacity
 *
 * @param $stroke
 */
?>
#data[<?php echo $join_field_name; ?> = '<?php echo $join_field_value; ?>'] {
    polygon-fill: <?php echo $color; ?>;
    polygon-opacity: <?php echo $opacity; ?>;
<?php if ($stroke['enable'] == '1'): ?>
    line-color: <?php echo         $stroke['color']; ?>;
    line-width: <?php echo   $stroke['width']; ?>;
    line-opacity: <?php echo $stroke['opacity']; ?>;
<?php endif; ?>
}
