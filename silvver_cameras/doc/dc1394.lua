require("silvver")

artkp_patt_dir = 'artkpImgs/4x4patt/'

scene = {

    cameras = {

        Dc1394{
            uid_suffix = "cam1",

            uid = dragonfly2uid(5110432),

            color_mode = 'mono8',
            bayer_method = "bilinear",
            color_filter = "rggb",
            resolution = {640, 480},
            frame_rate = '30',

            focal_length = {531.206054688, 531.451049805},
            principal_point = {359.263824463, 278.308197021},
            radial_coef = {-0.432087182999, 0.236846134067, 0},
            tangential_coef = {-0.000540043809451, 0.00384958018549},

            translation_vector = {358.432564738, -2.37973493006, 921.374295591},
            rotation_matrix = {
                0.99601358367, 0.00504113579782, -0.0890591269582,
                0.00133168260279, -0.999130874915, -0.0416619900223,
               -0.0891917471863, 0.041377309495, -0.995154636473
            },
        },
    },

    targets = {

        ARTKP{
            pattern_width = 50,
            threshold = 150,
            uid_prefix = "target1",

            {
                pattern_file = artkp_patt_dir .. '4x4_1.patt',
                uid = 1,
            },
        },
    },
}
