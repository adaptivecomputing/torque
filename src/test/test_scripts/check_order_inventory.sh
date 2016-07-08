#!/bin/bash

cat<<EOF
<?xml version="1.0"?>
<BasilResponse protocol="1.7">
 <ResponseData status="SUCCESS" method="QUERY" type="INVENTORY">
  <Inventory mpp_host="UNKNOWN" timestamp="1467404378" cpcu="0">
   <NodeArray changecount="1" schedchangecount="1">
    <Node node_id="8" name="c0-0c0s2n0" architecture="XT" role="BATCH" state="UP">
     <SocketArray>
      <Socket ordinal="0" architecture="x86_64" clock_mhz="1200">
       <SegmentArray>
        <Segment ordinal="0">
         <ComputeUnitArray>
          <ComputeUnit ordinal="0">
           <ProcessorArray>
            <Processor ordinal="0"/>
            <Processor ordinal="1"/>
            <Processor ordinal="2"/>
            <Processor ordinal="3"/>
           </ProcessorArray>
          </ComputeUnit>
         </ComputeUnitArray>
         <MemoryArray>
          <Memory type="OS" page_size_kb="4" page_count="25165824"/>
         </MemoryArray>
         <LabelArray/>
        </Segment>
       </SegmentArray>
      </Socket>
     </SocketArray>
    </Node>
   </NodeArray>
   <ReservationArray/>
  </Inventory>
 </ResponseData>
</BasilResponse>
EOF
