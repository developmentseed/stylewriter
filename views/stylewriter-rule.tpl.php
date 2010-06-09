<?php
// $Id$
/**
 * Style a Mapnik Rule
 *
 * @param $join_field
 * @param $value_field
 * @param $value
 *
 * @param $low
 * @param $high
 * @param $color
 */
?>
<Rule>
  <Filter>[<?php echo $join_field; ?>] &eq; <?php echo $value; ?></Filter>
  <Filter>[<?php echo $value_field; ?>] &gt; <?php echo $low; ?> and [<?php echo $value_field; ?>] &lt; <?php echo $high; ?></Filter>
  <PolygonSymbolizer>
  <CssParameter name="fill"><?php echo $color; ?></CssParameter>
  </PolygonSymbolizer>
  <LineSymbolizer>
    <CssParameter name="stroke">rgb(50%,50%,100%)</CssParameter>
    <CssParameter name="stroke-width">1</CssParameter>
  </LineSymbolizer>
</Rule>
