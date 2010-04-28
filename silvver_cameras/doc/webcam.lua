require("silvver")

artkp_patt_dir = 'artkpImgs/4x4patt/'

scene = {

    cameras = {

        V4l2{
            uid = 0,

            uid_suffix = "cam1",

            focal_length = {531.206054688, 531.451049805},
            principal_point = {359.263824463, 278.308197021},
            radial_coef = {0, 0, 0},
            tangential_coef = {0, 0},

            color_mode = 'yuyv',
            resolution = {640, 480},
        },
    },

    targets = {

        ARTKP{
            pattern_width = 50,
            threshold = 150,
            uid_prefix = "target1",

            body_pose = target2body({0,0,0},{0,-1, 0,
                                             1, 0, 0,
                                             0, 0, 1}),

            {
                pattern_file = artkp_patt_dir .. '4x4_1.patt',
                uid = 1,
            },
        },
    },
}
