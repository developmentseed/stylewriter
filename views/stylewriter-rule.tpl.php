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
<Rule>
  <Filter>[<?php echo $join_field_name; ?>] = '<?php echo $join_field_value; ?>'</Filter>
  <PolygonSymbolizer>
    <CssParameter name="fill"><?php echo $color; ?></CssParameter>
    <CssParameter name="fill-opacity"><?php echo $opacity; ?></CssParameter>
  </PolygonSymbolizer>
  <?php if ($stroke['enable'] == '1'): ?>
  <LineSymbolizer>
    <CssParameter name="stroke"><?php echo         $stroke['color']; ?></CssParameter>
    <CssParameter name="stroke-width"><?php echo   $stroke['width']; ?></CssParameter>
    <CssParameter name="stroke-opacity"><?php echo $stroke['opacity']; ?></CssParameter>
  </LineSymbolizer>
  <?php endif; ?>
</Rule>
