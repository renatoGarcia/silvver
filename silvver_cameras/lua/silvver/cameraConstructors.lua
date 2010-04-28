-- Copyright 2009,2010 Renato Florentino Garcia <fgar.renato@gmail.com>
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

do
    local automaticUid = 0
    function __getOptionalUid(value)
        if value then
            return value
        else
            automaticUid = automaticUid + 1
            return '_silvverInternal_camera' .. automaticUid
        end
    end
end

function PseudoCamera(parameters)
    camera = {}
    camera.__driver = 'pseudocamera'
    camera.uid_suffix = __getOptionalUid(parameters.uid_suffix)

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

function Dc1394(parameters)
    camera = {}
    camera.__driver = 'dc1394'
    camera.uid_suffix = __getOptionalUid(parameters.uid_suffix)

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

    camera.resolution = parameters.resolution
    camera.frame_rate = parameters.frame_rate
    camera.color_mode = parameters.color_mode
    camera.bayer_method = parameters.bayer_method -- Optional
    camera.color_filter = parameters.color_filter -- Optional

    -- The values below are optionals, they can be nil
    camera.brightness = parameters.brightness
    camera.exposure = parameters.exposure
    camera.sharpness = parameters.sharpness
    camera.hue = parameters.hue
    camera.saturation = parameters.saturation
    camera.gamma = parameters.gamma
    camera.shutter = parameters.shutter
    camera.gain = parameters.gain
    camera.iris = parameters.iris
    camera.focus = parameters.focus
    camera.temperature = parameters.temperature
    camera.trigger = parameters.trigger
    camera.trigger_delay = parameters.trigger_delay
    camera.zoom = parameters.zoom
    camera.pan = parameters.pan
    camera.tilt = parameters.tilt
    camera.optical_filter = parameters.optical_filter
    camera.capture_size = parameters.capture_size
    camera.capture_quality = parameters.capture_quality

    if parameters.white_balance == "off" then
        camera.white_balance = {"off","off"}
    elseif parameters.white_balance == "auto" then
        camera.white_balance = {"auto","auto"}
    else
        camera.white_balance = parameters.white_balance
    end

    if parameters.white_shading == "off" then
        camera.white_shading = {"off", "off", "off"}
    elseif parameters.white_shading == "auto" then
        camera.white_shading = {"auto", "auto", "auto"}
    else
        camera.white_shading = parameters.white_shading
    end

    return camera
end

function V4l2(parameters)
    camera = {}
    camera.__driver = 'v4l2'

    camera.save_image_format = __getOptional(parameters.save_image_format,
                                             "%1%_%2%_%3%.jpg")
    camera.uid_suffix = __getOptionalUid(parameters.uid_suffix)
    camera.uid = parameters.uid

    camera.focal_length = parameters.focal_length
    camera.principal_point = parameters.principal_point
    camera.radial_coef = parameters.radial_coef
    camera.tangential_coef = parameters.tangential_coef

    camera.color_mode = parameters.color_mode

    camera.translation_vector = __getOptional(parameters.translation_vector,
                                              {0, 0, 0})
    camera.rotation_matrix = __getOptional(parameters.rotation_matrix,
                                           {1, 0, 0,
                                            0, 1, 0,
                                            0, 0, 1})

    camera.resolution = parameters.resolution

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

function Dragonfly(parameters)
    camera = {}
    camera.__driver = 'dc1394'
    camera.uid_suffix = __getOptionalUid(parameters.uid_suffix)

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
    camera.bayer_method = __getOptional(parameters.bayer_method, 'bilinear')
    camera.color_filter = __getOptional(parameters.color_filter, 'rggb')

    -- The values below are optionals, they can be nil
    camera.brightness = parameters.brightness
    camera.exposure = parameters.exposure
    camera.sharpness = parameters.sharpness
    camera.hue = parameters.hue
    camera.saturation = parameters.saturation
    camera.gamma = parameters.gamma
    camera.shutter = parameters.shutter
    camera.gain = parameters.gain
    camera.iris = parameters.iris
    camera.focus = parameters.focus
    camera.temperature = parameters.temperature
    camera.trigger = parameters.trigger
    camera.trigger_delay = parameters.trigger_delay
    camera.zoom = parameters.zoom
    camera.pan = parameters.pan
    camera.tilt = parameters.tilt
    camera.optical_filter = parameters.optical_filter
    camera.capture_size = parameters.capture_size
    camera.capture_quality = parameters.capture_quality

    if parameters.white_balance == "off" then
        camera.white_balance = {"off","off"}
    elseif parameters.white_balance == "auto" then
        camera.white_balance = {"auto","auto"}
    else
        camera.white_balance = parameters.white_balance
    end

    if parameters.white_shading == "off" then
        camera.white_shading = {"off", "off", "off"}
    elseif parameters.white_shading == "auto" then
        camera.white_shading = {"auto", "auto", "auto"}
    else
        camera.white_shading = parameters.white_shading
    end

    return camera
end
