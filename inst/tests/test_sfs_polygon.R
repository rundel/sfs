test_that("signed area / direction", {
    poly_cw  = cbind(x=c(0,0,1,1,0), y=c(0,1,1,0,0))
    poly_ccw = cbind(x=c(0,1,1,0,0), y=c(0,0,1,1,0))
    poly_bad = cbind(x=c(0,1,1,0), y=c(0,0,1,1)) 

    expect_equal(polygon_signed_area(poly_cw), -1)
    expect_equal(polygon_signed_area(poly_ccw), 1)
    expect_error(polygon_signed_area(poly_bad))

    expect_true(is_cw(poly_cw))
    expect_false(is_cw(poly_ccw))
    expect_error(is_cw(poly_bad))
})