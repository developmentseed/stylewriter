<?php
// $Id$
/**
 * Style a Mapnik Rule
 *
 * @param $join_field_name
 * @param $join_field_value
 *
 * @param $color
 */
?>
<Rule>
  <Filter>[<?php echo $join_field_name; ?>] = '<?php echo $join_field_value; ?>'</Filter>
  <PolygonSymbolizer>
    <CssParameter name="fill"><?php echo $color; ?></CssParameter>
    <CssParameter name="fill-opacity">0.4</CssParameter>
  </PolygonSymbolizer>
</Rule>
