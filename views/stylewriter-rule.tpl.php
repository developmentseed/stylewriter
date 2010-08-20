<?php
// $Id$
/**
 * Style a Mapnik Rule
 *
 * @param $join_field_name
 * @param $join_field_value
 *
 * @param $polygon
 * @param $stroke
 * @param $metawriter
 * @param $scale
 * TODO: standardize so that this is a straight
 * conversion from an array
 */
?>
#data[<?php echo $filter; ?>]{

/* polygon */

<?php if ($polygon): ?>
  <?php foreach($polygon as $k => $v): ?>
    <?php echo "$k: $v;"; ?>
  <?php endforeach; ?>
<?php endif; ?>

/* stroke */

<?php if ($stroke['enable'] == '1'): ?>
  <?php unset($stroke['enable']); ?>
  <?php foreach($stroke as $k => $v): ?>
    <?php echo "$k: $v;"; ?>
  <?php endforeach; ?>
<?php endif; ?>

/* metawriter */

<?php /* if ($metawriter): ?>
<?php unset($metawriter['enable']); ?>
  <?php foreach($metawriter as $k => $v): ?>
    <?php echo "$k: \"$v\";"; ?>
  <?php endforeach; ?>
<?php endif; */ ?>

/* point */

<?php if ($point): ?>
<?php unset($point['enable']); ?>
  <?php foreach($point as $k => $v): ?>
    <?php echo "$k: $v;"; ?>
  <?php endforeach; ?>
  point-file: url('http://cascadenik-sampledata.s3.amazonaws.com/purple-point.png');
<?php endif; ?>

/* marker */
<?php if ($marker): ?>
<?php unset($marker['enable']); ?>
  <?php foreach($marker as $k => $v): ?>
    <?php echo "$k: $v;"; ?>
  <?php endforeach; ?>
<?php endif; ?>
}
