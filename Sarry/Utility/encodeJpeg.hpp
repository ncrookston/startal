#ifndef SARRY_UTILITY_ENCODEJPEG_HPP
#define SARRY_UTILITY_ENCODEJPEG_HPP

#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/scope_exit.hpp>
#include <boost/shared_array.hpp>
#include <vector>

namespace Sarry
{
  template <typename View>
  inline
  std::pair<boost::shared_array<boost::uint8_t>, std::size_t> encodeJpeg(
      const View& view, int quality = 100)
  {
    BOOST_STATIC_ASSERT(boost::gil::jpeg_write_support<View>::is_supported);

    jpeg_compress_struct cinfo;
    jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    BOOST_SCOPE_EXIT((&cinfo))
    { jpeg_destroy_compress(&cinfo); }
    BOOST_SCOPE_EXIT_END;

    unsigned char* pOutbuffer = NULL;
    unsigned long outlen;
    jpeg_mem_dest(&cinfo, &pOutbuffer, &outlen);
    boost::shared_array<boost::uint8_t> pOut(pOutbuffer, free);

    cinfo.image_width  = (JDIMENSION)view.width();
    cinfo.image_height = (JDIMENSION)view.height();
    cinfo.input_components = boost::gil::num_channels<View>::value;

    cinfo.in_color_space = boost::gil::detail::jpeg_write_support_private<
      typename boost::gil::channel_type<View>::type,
      typename boost::gil::color_space_type<View>::type>::color_type;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    jpeg_start_compress(&cinfo, TRUE);
    BOOST_SCOPE_EXIT((&cinfo))
    { jpeg_finish_compress(&cinfo); }
    BOOST_SCOPE_EXIT_END;

    std::vector<boost::gil::pixel<boost::gil::bits8,
      boost::gil::layout<typename boost::gil::color_space_type<View>::type> > >
      row(view.width());
    JSAMPLE* row_address = (JSAMPLE*)&row[0];

    for (int y = 0; y < view.height(); ++y)
    {
      std::copy(view.row_begin(y), view.row_end(y), row.begin());
      if(jpeg_write_scanlines(&cinfo,(JSAMPARRAY)&row_address, 1) != 1)
        throw std::runtime_error("Unable to write JPEG.");
    }

    return std::make_pair(pOut, outlen);
  }
}//end Sarry

#endif
