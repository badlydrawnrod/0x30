Module['preRun'] = [
    function() {
        FS.mkdir('/data');
        FS.mount(IDBFS, {}, '/data');
    },
];