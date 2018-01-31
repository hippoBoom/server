var express = require('express');
var router = express.Router();
var fs = require('fs')
var path = require('path')

router.get('/', (req, res, next) => {
    let tempArr = []
    let filePath = path.resolve(__dirname, '..')
    filePath = `${filePath}/static/servicedownload`

    let fileList = fs.readdirSync(filePath)

    fileList.forEach((item) => {
        let url = `${filePath}/${item}`
        let stat = fs.statSync(url)
        stat.name = item
        stat.url = url
        tempArr.push(stat)
    })

    res.json({
        status: 1,
        msg: 'success',
        result: {
            list: tempArr
        }
    })

})

module.exports = router;