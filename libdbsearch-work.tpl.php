<?php
/*
 * @file
 * Libdbsearch result page work template.
 * 
 * $rows_data           Work information data rows
 * $part_header         Part work headers
 * $part_data           Part work data
 * $work_image          Work image, link
 * $work_all_info       Link to work's all info
 * $work_content        Link to content / songs
 * $work_avail_info     Retun link from all info view
 * $work_introduction   Introduction of work from external store, link
 * $lib_avail           Availability oin chosen city library
 * $all_avail           Availability in all libraries within consortio
 * $work_reserve        Reserve work -link
 * $work_elink          Borrow EBook -link
 * $work_tip_link       Read / write a tip from work -link
 */
?>

<table class="footable tablet footable-loaded">
  <tbody>
    <?php print $rows_data; ?>
  </tbody>
</table>

<?php if (isset($part_data)) { ?>
  <h3><?php print $part_header; ?></h3>
  <table>
    <tbody>
      <?php print $part_data; ?>
    </tbody>
  </table>
<?php } ?>
<?php if (isset($part_info)) { ?>
  <h3><?php print $part_info_title; ?></h3>
  <table>
    <tbody>
      <?php print $part_info; ?>
    </tbody>
  </table>
<?php } ?>
    
<?php if (isset($work_image)) { print $work_image; } ?>
<?php if (isset($work_all_info)) { print $work_all_info; } ?>
<?php if (isset($work_content)) { print $work_content; } ?>
<?php if (isset($work_avail_info)) { print $work_avail_info; } ?>
<?php if (isset($work_introduction)) { print $work_introduction; } ?>

<?php if (isset($lib_avail)) { print $lib_avail; } ?>
<?php if (isset($all_avail)) { print $all_avail; } ?>

<?php if (isset($work_reserve)) { print $work_reserve; } ?>
<?php if (isset($work_elink)) { print $work_elink; } ?>
<?php if (isset($work_tip_link)) { print $work_tip_link; } ?>
