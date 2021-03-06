var express = require('express');
var path = require('path');
var favicon = require('serve-favicon');
var logger = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser = require('body-parser');
var history = require('connect-history-api-fallback');


var faqs = require('./routes/faqs');
var works = require('./routes/works')
var citems = require('./routes/citems')
var casebgs = require('./routes/casebgs')
var products = require('./routes/products')
var prodinfos = require('./routes/prodinfos')
var datadownload = require('./routes/datadownload')
var servicedownload = require('./routes/servicedownload')

var app = express();
app.use(history());

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');

// uncomment after placing your favicon in /public
//app.use(favicon(path.join(__dirname, 'public', 'favicon.ico')));
app.use(logger('dev'));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({extended: false}));
app.use(cookieParser());
// app.use(express.static(path.join(__dirname, 'public')));
app.use(express.static(path.join(__dirname, 'views')));

app.use('/faqs', faqs);
app.use('/works', works);
app.use('/citems', citems);
app.use('/casebgs', casebgs);
app.use('/products', products);
app.use('/prodinfos', prodinfos);
app.use('/datadownload', datadownload);
app.use('/servicedownload', servicedownload);


// catch 404 and forward to error handler
app.use(function (req, res, next) {
    var err = new Error('Not Found');
    err.status = 404;
    next(err);
});

// error handler
app.use(function (err, req, res, next) {
    // set locals, only providing error in development
    res.locals.message = err.message;
    res.locals.error = req.app.get('env') === 'development' ? err : {};

    // render the error page
    res.status(err.status || 500);
    res.render('error');
});

module.exports = app;
