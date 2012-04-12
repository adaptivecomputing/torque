#!/bin/bash


echo "<BasilResponse protocol='1.0'>
 <ResponseData status='SUCCESS' method='QUERY'>
  <Inventory>
   <NodeArray>
    <Node node_id='6142' name='c3-0c0s0n0' architecture='XT' role='BATCH' state='UP'>
     <SegmentArray>
      <Segment ordinal='0'>
       <ProcessorArray>
        <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='1' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='2' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='3' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='4' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='5' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='6' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='7' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='8' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='9' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='10' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='11' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
       </ProcessorArray>
       <MemoryArray>
        <Memory type='OS' page_size_kb='4' page_count='4096000'>
         <MemoryAllocation reservation_id='2869' page_count='6824960'/>
        </Memory>
       </MemoryArray>
      </Segment>
      <Segment>
       <ProcessorArray>
        <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='1' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='2' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='3' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='4' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='5' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='6' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='7' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='8' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='9' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='10' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='11' architecture='x86_64' clock_mhz='2100'/>
       </ProcessorArray>
       <MemoryArray>
        <Memory type='OS' page_size_kb='4' page_count='4096000'>
         <MemoryAllocation reservation_id='2869' page_count='6824960'/>
        </Memory>
       </MemoryArray>
      </Segment>
     </SegmentArray>
     <AcceleratorArray>
      <Accelerator ordinal='0' type='GPU' state='UP' family='E10' memory_mb='4000' clock_mhz='1000' />
      <Accelerator ordinal='1' type='GPU' state='UP' family='E10' memory_mb='4000' clock_mhz='1000' />
     </AcceleratorArray>
     <LabelArray>
      <Label name='MOAB:FEATURE=regmem' type='SOFT' disposition='ATTRACT'/>
      <Label name='MOAB:FEATURE=cheeseburger' type='SOFT' disposition='ATTRACT'/>
     </LabelArray>
    </Node>
    <Node node_id='6143' name='c3-0c0s0n1' architecture='XT' role='BATCH' state='UP'>
     <SegmentArray>
      <Segment ordinal='0'>
       <ProcessorArray>
        <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='1' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='2' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='3' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='4' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='5' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='6' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='7' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='8' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='9' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='10' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='11' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
       </ProcessorArray>
       <MemoryArray>
        <Memory type='OS' page_size_kb='4' page_count='4096000'>
         <MemoryAllocation reservation_id='2869' page_count='6824960'/>
        </Memory>
       </MemoryArray>
      </Segment>
      <Segment>
       <ProcessorArray>
        <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='1' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='2' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='3' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='4' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='5' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='6' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='7' architecture='x86_64' clock_mhz='2100'>
         <ProcessorAllocation reservation_id='2869'/>
        </Processor>
        <Processor ordinal='8' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='9' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='10' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='11' architecture='x86_64' clock_mhz='2100'/>
       </ProcessorArray>
       <MemoryArray>
        <Memory type='OS' page_size_kb='4' page_count='4096000'>
         <MemoryAllocation reservation_id='2869' page_count='6824960'/>
        </Memory>
       </MemoryArray>
      </Segment>
     </SegmentArray>
     <AcceleratorArray>
      <Accelerator ordinal='0' type='GPU' state='UP' family='E10' memory_mb='4000' clock_mhz='1000' />
      <Accelerator ordinal='1' type='GPU' state='UP' family='E10' memory_mb='4000' clock_mhz='1000' />
     </AcceleratorArray>
     <LabelArray>
      <Label name='MOAB:FEATURE=regmem' type='SOFT' disposition='ATTRACT'/>
     </LabelArray>
    </Node>
    <Node node_id='6144' name='c3-0c0s0n2' architecture='XT' role='BATCH' state='UP'>
     <SegmentArray>
      <Segment ordinal='0'>
       <ProcessorArray>
        <Processor ordinal='0' architecture='x86_64' clock_mhz='2100' />
        <Processor ordinal='1' architecture='x86_64' clock_mhz='2100' />
        <Processor ordinal='2' architecture='x86_64' clock_mhz='2100' />
        <Processor ordinal='3' architecture='x86_64' clock_mhz='2100' />
        <Processor ordinal='4' architecture='x86_64' clock_mhz='2100' />
        <Processor ordinal='5' architecture='x86_64' clock_mhz='2100' />
        <Processor ordinal='6' architecture='x86_64' clock_mhz='2100' />
        <Processor ordinal='7' architecture='x86_64' clock_mhz='2100' />
        <Processor ordinal='8' architecture='x86_64' clock_mhz='2100' />
        <Processor ordinal='9' architecture='x86_64' clock_mhz='2100' />
        <Processor ordinal='10' architecture='x86_64' clock_mhz='2100' />
        <Processor ordinal='11' architecture='x86_64' clock_mhz='2100' />
       </ProcessorArray>
       <MemoryArray>
        <Memory type='OS' page_size_kb='4' page_count='4096000'>
         <MemoryAllocation reservation_id='2869' page_count='6824960'/>
        </Memory>
       </MemoryArray>
      </Segment>
      <Segment>
       <ProcessorArray>
        <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='1' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='2' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='3' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='4' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='5' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='6' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='7' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='8' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='9' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='10' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='11' architecture='x86_64' clock_mhz='2100'/>
       </ProcessorArray>
       <MemoryArray>
        <Memory type='OS' page_size_kb='4' page_count='4096000'>
         <MemoryAllocation reservation_id='2869' page_count='6824960'/>
        </Memory>
       </MemoryArray>
      </Segment>
     </SegmentArray>
     <AcceleratorArray>
      <Accelerator ordinal='0' type='GPU' state='UP' family='E10' memory_mb='4000' clock_mhz='1000' />
      <Accelerator ordinal='1' type='GPU' state='UP' family='E10' memory_mb='4000' clock_mhz='1000' />
     </AcceleratorArray>
     <LabelArray>
      <Label name='MOAB:FEATURE=regmem' type='SOFT' disposition='ATTRACT'/>
     </LabelArray>
    </Node>
    <Node node_id='6145' name='c3-0c0s0n3' architecture='XT' role='BATCH' state='UP'>
     <SegmentArray>
      <Segment ordinal='0'>
       <ProcessorArray>
        <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='1' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='2' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='3' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='4' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='5' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='6' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='7' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='8' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='9' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='10' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='11' architecture='x86_64' clock_mhz='2100'/>
       </ProcessorArray>
       <MemoryArray>
        <Memory type='OS' page_size_kb='4' page_count='4096000'>
         <MemoryAllocation reservation_id='2869' page_count='6824960'/>
        </Memory>
       </MemoryArray>
      </Segment>
      <Segment>
       <ProcessorArray>
        <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='1' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='2' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='3' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='4' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='5' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='6' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='7' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='8' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='9' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='10' architecture='x86_64' clock_mhz='2100'/>
        <Processor ordinal='11' architecture='x86_64' clock_mhz='2100'/>
       </ProcessorArray>
       <MemoryArray>
        <Memory type='OS' page_size_kb='4' page_count='4096000'>
         <MemoryAllocation reservation_id='2869' page_count='6824960'/>
        </Memory>
       </MemoryArray>
      </Segment>
     </SegmentArray>
     <AcceleratorArray>
      <Accelerator ordinal='0' type='GPU' state='UP' family='E10' memory_mb='4000' clock_mhz='1000' />
      <Accelerator ordinal='1' type='GPU' state='UP' family='E10' memory_mb='4000' clock_mhz='1000' />
     </AcceleratorArray>
     <LabelArray>
      <Label name='MOAB:FEATURE=regmem' type='SOFT' disposition='ATTRACT'/>
     </LabelArray>
    </Node>
   </NodeArray>
   <ReservationArray>
    <Reservation reservation_id='1772' user_name='pgarias' account_name='DEFAULT'/>
    <Reservation reservation_id='2549' user_name='daryal' account_name='DEFAULT'/>
    <Reservation reservation_id='2869' user_name='zachary' account_name='DEFAULT'/>
   </ReservationArray>
  </Inventory>
 </ResponseData>
</BasilResponse>"

