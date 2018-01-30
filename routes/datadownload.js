var express = require('express');
var router = express.Router();
var fs = require('fs');
var path = require('path');

router.get('/', (req, res, next) => {
    let tempArr = []
    let id = req.param('id');
    let filePath = path.resolve(__dirname, '..');
    filePath = `${filePath}/static/datadownloads/${id}`;

    let fileList = fs.readdirSync(filePath);

    fileList.forEach((item, index) => {
        let stat = fs.statSync(`${filePath}/${item}`)
        stat.name = item
        tempArr.push(stat)
    })

    res.json({
        status: 1,
        msg: 'success',
        list: tempArr
    })

})

module.exports = router;

