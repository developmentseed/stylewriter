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
  </PolygonSymbolizer>
  <LineSymbolizer>
    <CssParameter name="stroke">rgb(50%,50%,100%)</CssParameter>
    <CssParameter name="stroke-width">1</CssParameter>
  </LineSymbolizer>
</Rule>
