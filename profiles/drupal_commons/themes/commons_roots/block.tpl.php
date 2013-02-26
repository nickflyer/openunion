<?php
// $Id: block.tpl.php 5639 2009-12-24 01:14:40Z chris $
?>

<div id="block-<?php print $block->module .'-'. $block->delta; ?>" class="block block-<?php print $block->module ?> <?php print $block_zebra; ?> <?php print $position; ?> <?php print $skinr; ?>">
  <div class="inner clearfix">
    <?php if (isset($edit_links)): ?>
    <?php print $edit_links; ?>
    <?php endif; ?>
    <?php if ($block->subject): ?>
    <div class="title block-title"><h2><span class="title-name"><?php print $block->subject ?></span>
        <?php if ($block->more): ?>
           <div class="title-morelink"><?php print $block->more ?></div>
        <?php endif;?>
</h2> 
    </div>
    <?php endif;?>
    <div class="content">
      <?php print $block->content ?>
    </div>
    <div class="block-bottom">
    </div>
  </div><!-- /block-inner -->
</div><!-- /block -->
