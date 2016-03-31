/**
 * Created by david on 31/03/2016.
 */

$(window).on('resize load', function () {
    $('body').css({"padding-top": $(".navbar").height() + 20 + "px"});
});
