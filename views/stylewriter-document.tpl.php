<?php
// $Id$
/**
 * @param $bgcolor 
 * @param $rules array of style rules
 */
?>
<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE Map>
<Map bgcolor="#000FFF" srs="+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null">
  <Style name="WorldBorders">
    <?php echo $rules; ?>
  </Style>
  <!-- Any layers named with the prefix (NIKTILE*) will get data from the JSON applied -->
  <Layer name="NIKTILE" srs="+proj=latlong +datum=WGS84">
    <StyleName>WorldBorders</StyleName>
  </Layer>
</Map>
