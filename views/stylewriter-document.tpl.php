<?php
// $Id$
/**
 * @param $bgcolor 
 * @param $data_url 
 * @param $rules array of style rules
 * @param $data_type 
 */
?>
<?php echo '<?xml version="1.0" encoding="utf-8"?>'; ?>
<!DOCTYPE Map>
<Map bgcolor="transparent" srs="+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +no_defs +over">
  <Stylesheet><![CDATA[
    <?php echo $rules; ?>
  ]]></Stylesheet>
  <Layer id="data" name="data" srs="+proj=latlong +datum=WGS84">
    <StyleName>data</StyleName>
    <Datasource> 
      <Parameter name="file"><?php echo $data_url; ?></Parameter> 
      <Parameter name="type"><?php echo $data_type; ?></Parameter> 
      <Parameter name="id">data</Parameter> 
    </Datasource> 
  </Layer>
</Map>
