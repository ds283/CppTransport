/**
 * Created by david on 31/03/2016.
 */

// Arrange body padding to resize if navbar resizes
$(window).on('resize load', function () {
    $('body').css({"padding-top": $(".navbar").height() + 20 + "px"});
});

$(document).ready(function () {
    // Initialize DataTables
    $('[class~="sortable"]').DataTable({responsive: true, bAutoWidth: false, paging: false, dom: 't'});
    $('[class~="sortable-filterable"]').DataTable({responsive: true, bAutoWidth: false, paging: false, dom: 'fti'});
    $('[class~="sortable-pageable"]').DataTable({responsive: true, bAutoWidth: false, dom: 'lftip'});

    // Initialize all tooltips
    $('[data-toggle="tooltip"]').tooltip();

    // Initialize all popovers
    $('[data-toggle="popover"]').popover();
});
