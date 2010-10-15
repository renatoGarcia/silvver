require("silvver")

-- Attention! This path is relative to directory where you are when
-- executing silvver_cameras
artkp_patt_dir = 'artkpImgs/4x4patt/'

function output2table(command)
    files_table = {}
    output = io.popen(command)

    repeat
        file_path = output:read('*l')
        table.insert(files_table, file_path)
    until file_path == nil

    return files_table
end

scene = {

    hardcameras = {

        PseudoCamera{
            -- Attention! This path on 'ls ...' is relative to directory
            -- where you are when executing silvver_cameras
            images_path = output2table('ls -d artkpImgs/*.png'),

            hardcamera_uid = 1,

            resolution = {640, 480},
            frame_rate = 30,

            focal_length = {530.09343, 529.65974},
            principal_point = {320, 240},
            radial_coef = {0.0, 0.0, 0.0},
            tangential_coef = {0.0, 0.0},
        },
    },

    targetsets = {

        ARTKP{
            pattern_width = 50,
            threshold = 150,
            targetset_uid = 1,

            {
                pattern_file = artkp_patt_dir .. '4x4_1.patt',
                uid = 1,
            },
        },
    },
}
