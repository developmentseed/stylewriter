<?php
// $Id$

/**
 * @param $color
 * @param $value
 * @param $place
 * @param $dist
 */
?>
<div class='stylewriter-swatch stylewriter-swatch-place-<?php echo $place; ?>'>
<?php if ($place == 'first'): ?>
  <span class='stylewriter-swatch-value'>
    <?php echo $value; ?>
  </span>
<?php endif; ?>
<div class='stylewriter-swatch-color 
  stylewriter-swatch-color-dist-<?php echo $dist; ?> 
  stylewriter-swatch-color-place-<?php echo $place; ?>' 
    style='background-color: <?php echo $color; ?>'>&nbsp;</div>
<?php if ($place == 'last'): ?>
  <span class='stylewriter-swatch-value'>
    <?php echo $value; ?>
  </span>
<?php endif; ?>
</div>
