/**
 * Created by david on 31/03/2016.
 */

// Arrange body padding to resize if navbar resizes
$(window).on('resize load', function () {
    $('body').css({"padding-top": $(".navbar").height() + 20 + "px"});
});

// Initialize all tooltips
$(function () {
    $('[data-toggle="tooltip"]').tooltip()
});

// Initialize all popovers
$(function () {
    $('[data-toggle="popover"]').popover()
});
