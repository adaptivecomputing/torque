#!/bin/bash


cat<<EOF
<BasilResponse protocol="1.3">
<ResponseData status="SUCCESS" method="QUERY">
<Inventory mpp_host="UNKNOWN" timestamp="1376500483">
<NodeArray changecount="1" schedchangecount="1">
<Node node_id="2" name="c0-0c0s1n0" architecture="XT" role="BATCH" state="UP">
<SocketArray>
<Socket ordinal="0" architecture="x86_64" clock_mhz="2300">
<SegmentArray>
<Segment ordinal="0">
<ComputeUnitArray>
<ComputeUnit ordinal="0">
<ProcessorArray>
<Processor ordinal="0"/>
<Processor ordinal="1"/>
</ProcessorArray>
</ComputeUnit>
<ComputeUnit ordinal="1">
<ProcessorArray>
<Processor ordinal="0"/>
<Processor ordinal="1"/>
</ProcessorArray>
</ComputeUnit>
<ComputeUnit ordinal="2">
<ProcessorArray>
<Processor ordinal="0"/>
<Processor ordinal="1"/>
</ProcessorArray>
</ComputeUnit>
<ComputeUnit ordinal="3">
<ProcessorArray>
<Processor ordinal="0"/>
<Processor ordinal="1"/>
</ProcessorArray>
</ComputeUnit>
</ComputeUnitArray>
<MemoryArray>
<Memory type="OS" page_size_kb="4" page_count="2097152"/>
</MemoryArray>
<LabelArray>
<Label name='MOAB:FEATURE=regmem' type='SOFT' disposition='ATTRACT'/>
<Label name='MOAB:FEATURE=cheeseburger' type='SOFT' disposition='ATTRACT'/>
</LabelArray>
</Segment>
<Segment ordinal="1">
<ComputeUnitArray>
<ComputeUnit ordinal="0">
<ProcessorArray>
<Processor ordinal="0"/>
<Processor ordinal="1"/>
</ProcessorArray>
</ComputeUnit>
<ComputeUnit ordinal="1">
<ProcessorArray>
<Processor ordinal="0"/>
<Processor ordinal="1"/>
</ProcessorArray>
</ComputeUnit>
<ComputeUnit ordinal="2">
<ProcessorArray>
<Processor ordinal="0"/>
<Processor ordinal="1"/>
</ProcessorArray>
</ComputeUnit>
<ComputeUnit ordinal="3">
<ProcessorArray>
<Processor ordinal="0"/>
<Processor ordinal="1"/>
</ProcessorArray>
</ComputeUnit>
</ComputeUnitArray>
<MemoryArray>
<Memory type="OS" page_size_kb="4" page_count="2097152"/>
</MemoryArray>
<LabelArray/>
</Segment>
</SegmentArray>
</Socket>
<Socket ordinal="1" architecture="x86_64" clock_mhz="2300">
<SegmentArray>
<Segment ordinal="0">
<ComputeUnitArray>
<ComputeUnit ordinal="0">
<ProcessorArray>
<Processor ordinal="0"/>
<Processor ordinal="1"/>
</ProcessorArray>
</ComputeUnit>
<ComputeUnit ordinal="1">
<ProcessorArray>
<Processor ordinal="0"/>
<Processor ordinal="1"/>
</ProcessorArray>
</ComputeUnit>
<ComputeUnit ordinal="2">
<ProcessorArray>
<Processor ordinal="0"/>
<Processor ordinal="1"/>
</ProcessorArray>
</ComputeUnit>
<ComputeUnit ordinal="3">
<ProcessorArray>
<Processor ordinal="0"/>
<Processor ordinal="1"/>
</ProcessorArray>
</ComputeUnit>
</ComputeUnitArray>
<MemoryArray>
<Memory type="OS" page_size_kb="4" page_count="2097152"/>
</MemoryArray>
<LabelArray/>
</Segment>
<Segment ordinal="1">...</Segment>
</SegmentArray>
</Socket>
</SocketArray>
<AcceleratorArray>
<Accelerator ordinal='0' type='GPU' state='UP' family='E10' memory_mb='4000' clock_mhz='1000' />
<Accelerator ordinal='1' type='GPU' state='UP' family='E10' memory_mb='4000' clock_mhz='1000' />
</AcceleratorArray>
</Node>
</NodeArray>
<ReservationArray/>
</Inventory>
</ResponseData>
</BasilResponse>
EOF
