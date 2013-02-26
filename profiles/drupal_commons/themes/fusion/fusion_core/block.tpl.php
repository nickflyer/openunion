<?php
?>

<div id="block-<?php print $block->module .'-'. $block->delta; ?>" class="block block-<?php print $block->module ?> <?php print $block_zebra; ?> <?php print $position; ?> <?php print $skinr; ?>">
  <div class="inner clearfix">
    <?php if (isset($edit_links)): ?>
    <?php print $edit_links; ?>
    <?php endif; ?>
    <?php if ($block->subject): ?>
    <div class="title block-title"><h2><span><?php print $block->subject ?></span></h2></div>
    <?php endif;?>
    <div class="content clearfix">
      <?php print $block->content ?>
    </div>
  </div><!-- /block-inner -->
</div><!-- /block -->
