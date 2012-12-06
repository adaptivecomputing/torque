#!/bin/bash

echo "<?xml version='1.0'?><BasilResponse protocol='1.1'><ResponseData status='SUCCESS' method='RESERVE'><Reserved reservation_id='$1'><ReservedNodeArray><ReservedNode node_id='9'/></ReservedNodeArray></Reserved></ResponseData></BasilResponse>"
