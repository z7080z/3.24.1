## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):
    obj = bld.create_ns3_module('lr-wpan', ['core', 'network', 'mobility', 'spectrum', 'propagation'])
    obj.source = [
    		'model/lr-wpan-beacon-header.cc',
    		'model/lr-wpan-command-frame.cc',
        'model/lr-wpan-error-model.cc',
        'model/lr-wpan-interference-helper.cc',
        'model/lr-wpan-phy.cc',
        'model/lr-wpan-mac.cc',
        'model/lr-wpan-mac-header.cc',
        'model/lr-wpan-mac-trailer.cc',
        'model/lr-wpan-csmaca.cc',
        'model/lr-wpan-net-device.cc',
        'model/lr-wpan-spectrum-value-helper.cc',
        'model/lr-wpan-spectrum-signal-parameters.cc',
        'model/lr-wpan-lqi-tag.cc',
        'helper/lr-wpan-helper.cc',
        ]

    module_test = bld.create_ns3_module_test_library('lr-wpan')
    module_test.source = [
        'test/lr-wpan-ack-test.cc',
        'test/lr-wpan-cca-test.cc',
        'test/lr-wpan-collision-test.cc',
        'test/lr-wpan-ed-test.cc',
        'test/lr-wpan-error-model-test.cc',
        'test/lr-wpan-packet-test.cc',
        'test/lr-wpan-pd-plme-sap-test.cc',
        'test/lr-wpan-spectrum-value-helper-test.cc',
        ]
     
    headers = bld(features='ns3header')
    headers.module = 'lr-wpan'
    headers.source = [
    		'model/lr-wpan-beacon-header.h',
    		'model/lr-wpan-command-frame.h',
        'model/lr-wpan-error-model.h',
        'model/lr-wpan-interference-helper.h',
        'model/lr-wpan-phy.h',
        'model/lr-wpan-mac.h',
        'model/lr-wpan-mac-header.h',
        'model/lr-wpan-mac-trailer.h',
        'model/lr-wpan-csmaca.h',
        'model/lr-wpan-net-device.h',
        'model/lr-wpan-spectrum-value-helper.h',
        'model/lr-wpan-spectrum-signal-parameters.h',
        'model/lr-wpan-lqi-tag.h',
        'helper/lr-wpan-helper.h',
        ]

    if (bld.env['ENABLE_EXAMPLES']):
        bld.recurse('examples')

    bld.ns3_python_bindings()
