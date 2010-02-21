-- Copyright 2009 Renato Florentino Garcia <fgar.renato@gmail.com>
--
-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License version 3, as
-- published by the Free Software Foundation.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.

function __getOptional(value, default)
    if value then
        return value
    else
        return default
    end
end

function PseudoCamera(parameters)
    camera = {}
    camera.__driver = 'pseudocamera'
    camera.identifier = string.gsub(parameters.images_path, "/", "_")

    camera.save_image_format = __getOptional(parameters.save_image_format,
                                             "%1%_%2%_%3%.jpg")
    camera.images_path = parameters.images_path

    camera.focal_length = parameters.focal_length
    camera.principal_point = parameters.principal_point
    camera.radial_coef = parameters.radial_coef
    camera.tangential_coef = parameters.tangential_coef

    camera.translation_vector = __getOptional(parameters.translation_vector,
                                              {0, 0, 0})
    camera.rotation_matrix = __getOptional(parameters.rotation_matrix,
                                           {1, 0, 0,
                                            0, 1, 0,
                                            0, 0, 1})

    camera.resolution = parameters.resolution
    camera.frame_rate = parameters.frame_rate

    return camera
end

function Dragonfly(parameters)
    camera = {}
    camera.__driver = 'dc1394'
    camera.identifier = parameters.uid

    camera.save_image_format = __getOptional(parameters.save_image_format,
                                             "%1%_%2%_%3%.jpg")
    camera.uid = parameters.uid

    camera.focal_length = parameters.focal_length
    camera.principal_point = parameters.principal_point
    camera.radial_coef = parameters.radial_coef
    camera.tangential_coef = parameters.tangential_coef

    camera.translation_vector = __getOptional(parameters.translation_vector,
                                              {0, 0, 0})
    camera.rotation_matrix = __getOptional(parameters.rotation_matrix,
                                           {1, 0, 0,
                                            0, 1, 0,
                                            0, 0, 1})

    camera.resolution = __getOptional(parameters.resolution, {640, 480})
    camera.frame_rate = __getOptional(parameters.frame_rate, 30)

    camera.color_mode = __getOptional(parameters.color_mode, 'mono8')
    camera.brightness = __getOptional(parameters.brightness, 'untouched')
    camera.exposure = __getOptional(parameters.exposure, 'untouched')
    camera.shutter = __getOptional(parameters.shutter, 'untouched')
    camera.gain = __getOptional(parameters.gain, 'untouched')
    camera.white_balance = __getOptional(parameters.white_balance,
                                         {'untouched', 'untouched'})

    return camera
end

function Webcam(parameters)
    camera = {}
    camera.__driver = 'v4l2'

    camera.save_image_format = __getOptional(parameters.save_image_format,
                                             "%1%_%2%_%3%.jpg")
    camera.identifier = parameters.uid

    camera.focal_length = parameters.focal_length
    camera.principal_point = parameters.principal_point
    camera.radial_coef = parameters.radial_coef
    camera.tangential_coef = parameters.tangential_coef

    camera.translation_vector = __getOptional(parameters.translation_vector,
                                              {0, 0, 0})
    camera.rotation_matrix = __getOptional(parameters.rotation_matrix,
                                           {1, 0, 0,
                                            0, 1, 0,
                                            0, 0, 1})

    camera.resolution = __getOptional(parameters.resolution, {640, 480})

    -- The values below are optionals, they can be nil
    camera.brightness                = parameters.brightness
    camera.contrast                  = parameters.contrast
    camera.saturation                = parameters.saturation
    camera.hue                       = parameters.hue
    camera.auto_white_balance        = parameters.auto_white_balance
    camera.red_balance               = parameters.red_balance
    camera.blue_balance              = parameters.blue_balance
    camera.gamma                     = parameters.gamma
    camera.exposure                  = parameters.exposure
    camera.autogain                  = parameters.autogain
    camera.gain                      = parameters.gain
    camera.horizontal_flip           = parameters.horizontal_flip
    camera.vertical_flip             = parameters.vertical_flip
    camera.power_line_frequency      = parameters.power_line_frequency
    camera.hue_auto                  = parameters.hue_auto
    camera.white_balance_temperature = parameters.white_balance_temperature
    camera.sharpness                 = parameters.sharpness
    camera.backlight_compensation    = parameters.backlight_compensation

    return camera
end