#!/bin/bash

echo "<?xml version='1.0'?><BasilResponse protocol='1.1'><ResponseData status='SUCCESS' method='CONFIRM'><Confirmed reservation_id='$1' pagg_id='$2'/></ResponseData></BasilResponse>"
