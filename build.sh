OUTPUT=shadow_client

rm -rf $OUTPUT

g++ -o $OUTPUT main.cpp /home/kali/d3m0n_os_debian/rootfs/usr/share/d3m0n/src/settings.cpp -I/usr/include/opencv4/opencv2  -std=c++17 -I. -lssl -lcrypto -lsqlite3 -I/usr/include/opencv4 -lopencv_stitching -lopencv_aruco -lopencv_bgsegm -lopencv_bioinspired -lopencv_ccalib -lopencv_dpm -lopencv_face -lopencv_freetype -lopencv_fuzzy -lopencv_hdf -lopencv_line_descriptor -lopencv_reg -lopencv_rgbd -lopencv_saliency -lopencv_shape -lopencv_stereo -lopencv_structured_light -lopencv_phase_unwrapping -lopencv_superres -lopencv_optflow -lopencv_surface_matching -lopencv_highgui -lopencv_datasets -lopencv_text -lopencv_plot -lopencv_ml -lopencv_videostab -lopencv_videoio -lopencv_viz -lopencv_ximgproc -lopencv_video -lopencv_xobjdetect -lopencv_objdetect -lopencv_calib3d -lopencv_imgcodecs -lopencv_features2d -lopencv_flann -lopencv_xphoto -lopencv_photo -lopencv_imgproc -lopencv_core || exit 1

chmod +x $OUTPUT

clear

./$OUTPUT $1 $2 $3