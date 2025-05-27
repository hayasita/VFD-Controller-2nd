module.exports = function(api) {
  api.cache(true);

  const presets = [
    [
      '@babel/preset-env',
      {
        targets: {
          browsers: ['iOS >= 12'],
        },
      },
    ],
  ];

  return {
    presets
  };
};
