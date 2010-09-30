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

/* marker */
<?php if ($marker): ?>
<?php unset($marker['enable']); ?>
  <?php foreach($marker as $k => $v): ?>
    <?php echo "$k: $v;"; ?>
  <?php endforeach; ?>
  <?php if ($metawriter['enable']): ?>
    marker-meta-writer: "meta1";
    marker-meta-output: "<?php echo ($metawriter['meta-output']); ?>";
  <?php endif; ?>
<?php endif; ?>

/* postfix */
<?php if($postfix) { echo $postfix; } ?>

}
