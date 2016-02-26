#!/bin/bash

echo "<BasilResponse protocol='1.7'>
 <ResponseData status='SUCCESS' method='SYSTEM'>
   <System mpp_host='napali' timestamp='123' cpcu='1'>
     <Nodes role='batch' state='up' speed='1200' numa_nodes='1' dies='1' compute_units='60' cpus_per_cu='4' page_size_kb='4' page_count='25165824' numa_cfg='a2a' hbm_size_mb='4000' hbm_cache_pct='0'>
       6142-6143
     </Nodes>
   </System>
 </ResponseData>
</BasilResponse>"
